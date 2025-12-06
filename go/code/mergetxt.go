package main

import (
	"bufio"
	"fmt"
	"log"
	"os"
)

// 合并txt文件，合并到一个文件中，按行去重
func mergeFiles(inputFiles []string, outputFile string) error {
	// 创建一个map来存储唯一的行
	uniqueLines := make(map[string]bool)

	// 遍历所有输入文件
	for _, inputFile := range inputFiles {
		file, err := os.Open(inputFile)
		if err != nil {
			return fmt.Errorf("failed to open input file %s: %w", inputFile, err)
		}
		defer file.Close()

		scanner := bufio.NewScanner(file)
		for scanner.Scan() {
			line := scanner.Text()
			uniqueLines[line] = true
		}
		if err := scanner.Err(); err != nil {
			return fmt.Errorf("failed to read input file %s: %w", inputFile, err)
		}
	}

	// 写入输出文件
	output, err := os.Create(outputFile)
	if err != nil {
		return fmt.Errorf("failed to create output file %s: %w", outputFile, err)
	}
	defer output.Close()

	writer := bufio.NewWriter(output)
	for line := range uniqueLines {
		_, err := fmt.Fprintln(writer, line)
		if err != nil {
			return fmt.Errorf("failed to write to output file %s: %w", outputFile, err)
		}
	}
	if err := writer.Flush(); err != nil {
		return fmt.Errorf("failed to flush output file %s: %w", outputFile, err)
	}

	return nil
}

func main() {
	if len(os.Args) < 3 {
		fmt.Println("Usage: merge <input_files> <output_file>")
		os.Exit(1)
	}

	inputFiles := os.Args[1 : len(os.Args)-1]
	outputFile := os.Args[len(os.Args)-1]

	if err := mergeFiles(inputFiles, outputFile); err != nil {
		log.Fatalf("Error merging files: %v", err)
	}
}
