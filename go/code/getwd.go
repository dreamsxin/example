package main

import (
	"log"
	"os"
	"os/exec"
	"path/filepath"
)

func main() {
	dir, _ := os.Getwd()
	log.Println("Getwd", dir)

	cmd := exec.Command(os.Args[0])
	path := filepath.Dir(cmd.Path)

	log.Println("path", path)

	execPath, _ := os.Executable()
	log.Println("execPath", filepath.Dir(execPath))
}
