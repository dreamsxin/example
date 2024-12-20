package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"path/filepath"
)

const defaultDirectory = ""
const defaultExtension = ""
const defaultHeader = ""
const defaultFooter = ""
const defaultDigits = 0

type arguments struct {
	directory	string
	extension	string
	header		string
	footer		string
	digits		int
	isTest		bool
}

func main() {
	args := fetchArgs()
	if args.extension != "" && args.extension[0] != '.' {
		args.extension = "." + args.extension
	}

	fileInfos, err := ioutil.ReadDir(args.directory)
	if err != nil {
		fmt.Println("Error:", err)
		return
	}

	fmt.Println("<-Before\t\tAfter->")
	serial := 0;
	for _, fileInfo := range fileInfos {
		if fileInfo.IsDir() {
			continue
		}
		if args.extension != "" && filepath.Ext(fileInfo.Name()) != args.extension {
			continue
		}

		format := "%s%"
		if args.digits != 0 {
			format += fmt.Sprintf("0%d", args.digits)
		}
		format += "d%s%s"
		newname := fmt.Sprintf(format, args.header, serial, args.footer, filepath.Ext(fileInfo.Name()))

		fmt.Printf("%s\t\t%s\n", fileInfo.Name(), newname)

		if !args.isTest {
			oldpath := filepath.Join(args.directory, fileInfo.Name())
			newpath := filepath.Join(args.directory, newname)
			if err := os.Rename(oldpath, newpath); err != nil {
				fmt.Println("Error:", err)
			}
		}
		serial++
	}
}

func fetchArgs() *arguments {
	args := new(arguments)
	flag.StringVar(&args.directory, "d", defaultDirectory, "Path of directory.")
	flag.StringVar(&args.extension, "e", defaultExtension, "Condition of file extension.")
	flag.StringVar(&args.header, "h", defaultHeader, "Header string of finename.")
	flag.StringVar(&args.footer, "f", defaultFooter, "Footer string of filename.")
	flag.IntVar(&args.digits, "n", defaultDigits, "Number of digits for serial number.")
	flag.BoolVar(&args.isTest, "t", false, "Output new file name only (will not rename).")
	flag.Parse()
	return args
}
