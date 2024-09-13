Scrap records your screen! At least it does if you're on Windows, macOS, or Linux.

- https://github.com/quadrupleslap/scrap

## Install
```shell
cargo add scrap
```

## 屏幕截图
```shell
cargo add repng
```

```rust
extern crate repng;
extern crate scrap;

use scrap::{Capturer, Display};
use std::io::ErrorKind::WouldBlock;
use std::fs::File;
use std::thread;
use std::time::Duration;

fn main() {
    let one_second = Duration::new(1, 0);
    let one_frame = one_second / 60;

    let display = Display::primary().expect("Couldn't find primary display.");
    let mut capturer = Capturer::new(display).expect("Couldn't begin capture.");
    let (w, h) = (capturer.width(), capturer.height());

    loop {
        // Wait until there's a frame.

        let buffer = match capturer.frame() {
            Ok(buffer) => buffer,
            Err(error) => {
                if error.kind() == WouldBlock {
                    // Keep spinning.
                    thread::sleep(one_frame);
                    continue;
                } else {
                    panic!("Error: {}", error);
                }
            }
        };

        println!("Captured! Saving...");

        // Flip the ARGB image into a BGRA image.

        let mut bitflipped = Vec::with_capacity(w * h * 4);
        let stride = buffer.len() / h;

        for y in 0..h {
            for x in 0..w {
                let i = stride * y + 4 * x;
                bitflipped.extend_from_slice(&[
                    buffer[i + 2],
                    buffer[i + 1],
                    buffer[i],
                    255,
                ]);
            }
        }

        // Save the image.

        repng::encode(
            File::create("screenshot.png").unwrap(),
            w as u32,
            h as u32,
            &bitflipped,
        ).unwrap();

        println!("Image saved to `screenshot.png`.");
        break;
    }
}
```

## 录制屏幕
安装 ffmpeg
```shell
choco install ffmpeg
```
使用 ffplay 播放 raw 数据 `ffplay -f rawvideo -s 512x512 -pix_fmt rgb24 test.raw`
```rust
fn main() {
    use scrap::{Capturer, Display};
    use std::io::Write;
    use std::io::ErrorKind::WouldBlock;
    use std::process::{Command, Stdio};

    let d = Display::primary().unwrap();
    let (w, h) = (d.width(), d.height());

    let child = Command::new("ffplay")
        .args(&[
            "-f", "rawvideo",
            "-pixel_format", "bgr0",
            "-video_size", &format!("{}x{}", w, h),
            "-framerate", "60",
            "-"
        ])
        .stdin(Stdio::piped())
        .spawn()
        .expect("This example requires ffplay.");

    let mut capturer = Capturer::new(d).unwrap();
    let mut out = child.stdin.unwrap();

    loop {
        match capturer.frame() {
            Ok(frame) => {
                // Write the frame, removing end-of-row padding.
                let stride = frame.len() / h;
                let rowlen = 4 * w;
                for row in frame.chunks(stride) {
                    let row = &row[..rowlen];
                    out.write_all(row).unwrap();
                }
            }
            Err(ref e) if e.kind() == WouldBlock => {
                // Wait for the frame.
            }
            Err(_) => {
                // We're done here.
                break;
            }
        }
    }
}
```

### 保存为 MP4

```shell
cargo add ffmpeg
```

```rust
use scrap::{Capturer, Display};
use std::time::Duration;
use ffmpeg::{format::context::Output, codec::encoder::Video, software::scaling::Context as SwsContext};
use ffmpeg::util::format::pixel::Pixel;
use std::fs::File;
use std::io::BufWriter;

fn main() {
    // 获取默认显示
    let display = Display::primary().unwrap();

    // 创建捕获器
    let mut capturer = Capturer::new(display).unwrap();

    // 设置输出文件
    let output_file = File::create("output.mp4").unwrap();
    let mut output = BufWriter::new(output_file);

    // 设置视频编码器
    let mut encoder = ffmpeg::encoder::find(ffmpeg::encoder::Id::MPEG4).unwrap();
    encoder.set_bit_rate(2_000_000);
    encoder.set_time_base(ffmpeg::util::rational::Rational { num: 1, den: 25 });
    encoder.set_pixel_format(Pixel::YUV420P);
    encoder.set_width(capturer.width());
    encoder.set_height(capturer.height());

    // 打开编码器
    encoder.open().unwrap();

    // 创建输出上下文
    let mut output_context = Output::new();
    output_context.add_stream(encoder).unwrap();

    // 打开输出文件
    output_context.open(&["output.mp4"], Some("mp4"), None).unwrap();

    // 创建视频编码器
    let mut video_stream = output_context.find_stream(0).unwrap();
    let video_encoder = video_stream.codec().encoder().unwrap();

    // 创建视频编码器上下文
    let mut video_encoder_context = video_encoder.open(&video_stream.codec().params().unwrap()).unwrap();

    // 创建视频编码器上下文
    let mut sws_context = SwsContext::new(
        Pixel::BGR24,
        Pixel::YUV420P,
        capturer.width(),
        capturer.height(),
        ffmpeg::util::frame::video::Flags::empty(),
    ).unwrap();

    // 开始录制
    loop {
        // 捕获一帧
        let frame = capturer.frame().unwrap();

        // 转换像素格式
        let mut converted_frame = ffmpeg::util::frame::video::Video::new(
            Pixel::YUV420P,
            capturer.width(),
            capturer.height(),
        );
        sws_context.run(&frame, &mut converted_frame).unwrap();

        // 编码帧
        let mut packet = ffmpeg::packet::Packet::empty();
        video_encoder_context.encode(&converted_frame, &mut packet).unwrap();

        // 写入输出文件
        if packet.is_empty() {
            continue;
        }
        packet.write_interleaved(&mut output_context).unwrap();


        // 按下 'q' 键退出
        if let Some(key) = video.window().wait_key(Duration::from_secs(0)) {
            if key == scrap::Key::Q {
                break;
            }
        }
    }
}
```
