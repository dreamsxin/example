#
- https://github.com/gonum/plot
- https://github.com/gonum/plot/wiki/Example-plots
- https://github.com/wcharczuk/go-chart

##

```go
package main

import (
	"log"
	"math/rand"
	"time"

	"gonum.org/v1/plot"
	"gonum.org/v1/plot/plotter"
	"gonum.org/v1/plot/plotutil"
	"gonum.org/v1/plot/vg"
)

func main() {

	// 线点图
	p := plot.New()

	p.Title.Text = "Get Started"
	p.X.Label.Text = "X"
	p.Y.Label.Text = "Y"

	err := plotutil.AddLinePoints(p,
		"First", randomPoints(15),
		"Second", randomPoints(15),
		"Third", randomPoints(15))
	if err != nil {
		log.Fatal(err)
	}

	if err = p.Save(4*vg.Inch, 4*vg.Inch, "linepoints.png"); err != nil {
		log.Fatal(err)
	}

	// 三点图
	p = plot.New()
	p.Title.Text = "Get Started"
	p.X.Label.Text = "X"
	p.Y.Label.Text = "Y"

	scatter2, _ := plotter.NewScatter(randomPoints(15))
	p.Add(scatter2)
	plotutil.AddScatters(p, "Second", randomPoints(15))

	if err = p.Save(4*vg.Inch, 4*vg.Inch, "scatter.png"); err != nil {
		log.Fatal(err)
	}
}

func randomPoints(n int) plotter.XYs {
	r := rand.New(rand.NewSource(time.Now().UnixMicro()))
	points := make(plotter.XYs, n)
	for i := range points {
		if i == 0 {
			points[i].X = r.Float64()
		} else {
			points[i].X = points[i-1].X + r.Float64()
		}
		points[i].Y = points[i].X + 10*r.Float64()
	}

	return points
}
```
