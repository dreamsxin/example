package main
 
import (
    "fmt"
    "log"
    "os"

    "image"
    "image/color"
    "image/png"
    "golang.org/x/image/webp"
    "golang.org/x/image/draw"
)
 
func main() {
    // This example uses png.Decode which can only decode PNG images.
    catFile, err := os.Open("cat.webp")
    if err != nil {
        log.Fatal(err)
    }
    defer catFile.Close()
 
    // Consider using the general image.Decode as it can sniff and decode any registered image format.
    img, err := webp.Decode(catFile)
    if err != nil {
        log.Fatal(err)
    }
 
    fmt.Println(img.Bounds().Dx(), img.Bounds().Dy())

	dst := image.NewRGBA(image.Rect(0, 0, img.Bounds().Dx()/10, img.Bounds().Dy()/10))
	draw.BiLinear.Scale(dst, dst.Bounds(), img, img.Bounds(), draw.Over, nil)

    fmt.Println(dst.Bounds().Dx(), dst.Bounds().Dy())

    f, err := os.Create("cat.png")
    if err != nil {
		log.Fatal(err)
	}
	defer f.Close()
	if err := png.Encode(f, dst); err != nil {
		log.Fatal(err)
	}

    levels := []string{".", "▪", "□", "●", "▲", "◼", "■"}
 
    for y := dst.Bounds().Min.Y; y < dst.Bounds().Max.Y; y++ {
        for x := dst.Bounds().Min.X; x < dst.Bounds().Max.X; x++ {
            c := color.GrayModel.Convert(dst.At(x, y)).(color.Gray)
            level := c.Y / 51 // 51 * 5 = 255
            fmt.Print(levels[level])
        }
        fmt.Print("\n")
    }
}
