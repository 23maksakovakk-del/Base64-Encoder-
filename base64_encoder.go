// base64_encoder.go
package main

import (
	"encoding/base64"
	"flag"
	"fmt"
	"io"
	"os"
	"path/filepath"
)

func progressBar(current, total int64, width int) {
	if total == 0 {
		return
	}
	percent := float64(current) / float64(total)
	filled := int(percent * float64(width))
	bar := ""
	for i := 0; i < filled; i++ {
		bar += "█"
	}
	for i := filled; i < width; i++ {
		bar += "░"
	}
	fmt.Fprintf(os.Stderr, "\rProgress: [%s] %.1f%%", bar, percent*100)
	if current >= total {
		fmt.Fprintln(os.Stderr)
	}
}

func encodeFile(inputPath, outputPath string, urlSafe bool) error {
	info, err := os.Stat(inputPath)
	if err != nil {
		return err
	}
	fileSize := info.Size()
	chunkSize := int64(1024 * 1024) // 1 MB
	out := os.Stdout
	if outputPath != "" {
		out, err = os.Create(outputPath)
		if err != nil {
			return err
		}
		defer out.Close()
	}
	f, err := os.Open(inputPath)
	if err != nil {
		return err
	}
	defer f.Close()
	encoder := base64.StdEncoding
	if urlSafe {
		encoder = base64.URLEncoding
	}
	buf := make([]byte, chunkSize)
	var processed int64
	for {
		n, err := f.Read(buf)
		if n > 0 {
			encoded := encoder.EncodeToString(buf[:n])
			_, werr := out.Write([]byte(encoded))
			if werr != nil {
				return werr
			}
			processed += int64(n)
			progressBar(processed, fileSize, 40)
		}
		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}
	}
	fmt.Printf("\n✅ Encoded '%s' to '%s'\n", inputPath, outputPath)
	return nil
}

func decodeFile(inputPath, outputPath string, urlSafe bool) error {
	data, err := os.ReadFile(inputPath)
	if err != nil {
		return err
	}
	// Remove whitespace
	clean := make([]byte, 0, len(data))
	for _, b := range data {
		if b != ' ' && b != '\n' && b != '\r' && b != '\t' {
			clean = append(clean, b)
		}
	}
	decoder := base64.StdEncoding
	if urlSafe {
		decoder = base64.URLEncoding
	}
	decoded, err := decoder.DecodeString(string(clean))
	if err != nil {
		return fmt.Errorf("invalid Base64: %w", err)
	}
	out := os.Stdout
	if outputPath != "" {
		out, err = os.Create(outputPath)
		if err != nil {
			return err
		}
		defer out.Close()
	}
	_, err = out.Write(decoded)
	if err != nil {
		return err
	}
	fmt.Printf("\n✅ Decoded '%s' to '%s'\n", inputPath, outputPath)
	return nil
}

func main() {
	var (
		encode   = flag.String("e", "", "Encode file")
		decode   = flag.String("d", "", "Decode file")
		output   = flag.String("o", "", "Output file")
		urlSafe  = flag.Bool("u", false, "Use URL-safe alphabet")
	)
	flag.Parse()
	if *encode == "" && *decode == "" {
		fmt.Fprintln(os.Stderr, "Error: either -e or -d required")
		flag.Usage()
		os.Exit(1)
	}
	var err error
	if *encode != "" {
		err = encodeFile(*encode, *output, *urlSafe)
	} else {
		err = decodeFile(*decode, *output, *urlSafe)
	}
	if err != nil {
		fmt.Fprintf(os.Stderr, "Error: %v\n", err)
		os.Exit(1)
	}
}
