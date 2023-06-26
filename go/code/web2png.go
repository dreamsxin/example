package main

import (
	"flag"
	"io"
	"log"
	"os"

	"image/png"

	"golang.org/x/image/webp"
)

func main() {
	inname  := flag.String("input",  "-", "can be - for standard in")
	outname := flag.String("output", "-", "can be - for standard out")
	flag.Parse()

	var (
		infile  io.Reader
		outfile io.Writer
	)

	if *inname == "-" {
		infile = os.Stdin
	} else {
		file, err := os.Open(*inname)
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()
		infile = file
	}

	if *outname == "-" {
		outfile = os.Stdout
	} else {
		flag := os.O_WRONLY | os.O_CREATE | os.O_TRUNC
		file, err := os.OpenFile(*outname, flag, 0666)
		if err != nil {
			log.Fatal(err)
		}
		defer file.Close()
		outfile = file
	}

	if m, err := webp.Decode(infile); err != nil {
		log.Fatal(err)
	} else {
		if err := png.Encode(outfile, m); err != nil {
			log.Fatal(err)
		}
	}
}
