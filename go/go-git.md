## git

```go
package main

import (
	"fmt"
	"os"
	"path/filepath"

	"github.com/go-git/go-git/v5"
	"github.com/go-git/go-git/v5/plumbing/object"
)

func main() {
	err := SearchLog("../../wa-version/html")
	if err != nil {
		fmt.Println(err)
		os.Exit(1)
	}
}
func SearchLog(dir string) error {
	files, err := os.ReadDir(dir)
	if err != nil {
		return err
	}
	for _, f := range files {
		if f.IsDir() {
			path := filepath.Join(dir, f.Name())
			r, err := git.PlainOpen(path)
			if err != nil {
				fmt.Printf("%s %v", f.Name(), err)
				continue
			}
			err = searchLogInRepo(r)
			if err != nil {
				return err
			}

		}
	}
	return nil
}

func searchLogInRepo(r *git.Repository) error {
	options := git.LogOptions{}
	cIter, err := r.Log(&options)
	if err != nil {
		return err
	}
	err = cIter.ForEach(func(c *object.Commit) error {
		fmt.Println(c)
		return nil
	})
	return err
}

```
