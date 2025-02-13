```go
package main

import (
	"bytes"
	"context"
	"encoding/base64"
	"encoding/binary"
	"image"
	_ "image/png"
	"io"
	"log"
	"os/exec"
	"time"

	"github.com/chromedp/cdproto/page"
	"github.com/chromedp/chromedp"
	"github.com/pion/webrtc/v3"
	"github.com/pion/webrtc/v3/pkg/media"
)

const (
	width  = 1280
	height = 720
	fps    = 30
)

func main() {
	// 初始化WebRTC
	config := webrtc.Configuration{
		ICEServers: []webrtc.ICEServer{
			{URLs: []string{"stun:stun.l.google.com:19302"}},
	}

	peerConnection, err := webrtc.NewPeerConnection(config)
	if err != nil {
		log.Fatal(err)
	}

	// 创建视频轨道
	videoTrack, err := webrtc.NewTrackLocalStaticSample(
		webrtc.RTPCodecCapability{MimeType: webrtc.MimeTypeVP8},
		"video",
		"screen-cast",
	)
	if err != nil {
		log.Fatal(err)
	}

	if _, err = peerConnection.AddTrack(videoTrack); err != nil {
		log.Fatal(err)
	}

	// 启动ffmpeg编码进程
	cmd := exec.Command("ffmpeg",
		"-f", "rawvideo",
		"-pix_fmt", "yuv420p",
		"-s", "1280x720",
		"-r", "30",
		"-i", "-",
		"-c:v", "libvpx",
		"-f", "ivf",
		"-deadline", "realtime",
		"-cpu-used", "4",
		"-",
	)

	ffmpegIn, _ := cmd.StdinPipe()
	ffmpegOut, _ := cmd.StdoutPipe()
	go func() {
		if err := cmd.Start(); err != nil {
			log.Fatal("ffmpeg启动失败:", err)
		}
	}()

	// 处理ffmpeg输出
	go processIVFStream(ffmpegOut, videoTrack)

	// 启动CDP连接
	ctx, cancel := chromedp.NewContext(context.Background())
	defer cancel()

	// 启动screencast
	if err := chromedp.Run(ctx,
		page.Enable(),
		page.StartScreencast(&page.StartScreencastParams{
			Format:          page.ScreencastFormatPng,
			Quality:         90,
			MaxWidth:        width,
			MaxHeight:       height,
			EveryNthFrame:   1,
		}),
	); err != nil {
		log.Fatal(err)
	}

	chromedp.ListenTarget(ctx, func(ev interface{}) {
		if ev, ok := ev.(*page.EventScreencastFrame); ok {
			go processFrame(ctx, ev, ffmpegIn)
		}
	})

	// 保持运行
	select {}
}

func processFrame(ctx context.Context, ev *page.EventScreencastFrame, writer io.Writer) {
	defer page.ScreencastFrameAck(ev.SessionID).Do(ctx)

	data, err := base64.StdEncoding.DecodeString(ev.Data)
	if err != nil {
		log.Println("base64解码失败:", err)
		return
	}

	img, _, err := image.Decode(bytes.NewReader(data))
	if err != nil {
		log.Println("图片解码失败:", err)
		return
	}

	yuv := convertToYUV420(img)
	if _, err := writer.Write(yuv); err != nil {
		log.Println("写入ffmpeg失败:", err)
	}
}

func convertToYUV420(img image.Image) []byte {
	bounds := img.Bounds()
	w, h := bounds.Dx(), bounds.Dy()
	ySize := w * h
	yuv := make([]byte, ySize*3/2)

	for y := 0; y < h; y++ {
		for x := 0; x < w; x++ {
			r, g, b, _ := img.At(x, y).RGBA()
			Y, U, V := rgbToYUV(uint8(r>>8), uint8(g>>8), uint8(b>>8))
			yuv[y*w+x] = Y

			if x%2 == 0 && y%2 == 0 {
				uvIndex := ySize + (y/2)*(w/2) + (x/2)
				yuv[uvIndex] = U
				yuv[uvIndex+ySize/4] = V
			}
		}
	}
	return yuv
}

func rgbToYUV(r, g, b uint8) (uint8, uint8, uint8) {
	y := (66*r + 129*g + 25*b + 128) >> 8 + 16
	u := (-38*r - 74*g + 112*b + 128) >> 8 + 128
	v := (112*r - 94*g - 18*b + 128) >> 8 + 128
	return clamp(y), clamp(u), clamp(v)
}

func clamp(value int) uint8 {
	if value < 0 {
		return 0
	}
	if value > 255 {
		return 255
	}
	return uint8(value)
}

func processIVFStream(reader io.Reader, track *webrtc.TrackLocalStaticSample) {
	header := make([]byte, 32)
	if _, err := io.ReadFull(reader, header); err != nil {
		log.Fatal("IVF头读取失败:", err)
	}

	for {
		frameHeader := make([]byte, 12)
		if _, err := io.ReadFull(reader, frameHeader); err != nil {
			log.Println("帧头读取失败:", err)
			break
		}

		frameSize := binary.LittleEndian.Uint32(frameHeader[0:4])
		frameData := make([]byte, frameSize)
		if _, err := io.ReadFull(reader, frameData); err != nil {
			log.Println("帧数据读取失败:", err)
			break
		}

		if err := track.WriteSample(media.Sample{
			Data:     frameData,
			Duration: time.Second / fps,
		}); err != nil {
			log.Println("样本写入失败:", err)
		}
	}
}
```
