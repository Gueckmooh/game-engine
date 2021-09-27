package util

import (
	"fmt"
	"io/ioutil"
	"path"
)

func FindUpDir(toFind string, dirname string) (string, error) {
	for true {
		fis, err := ioutil.ReadDir(dirname)
		if err != nil {
			return "", fmt.Errorf("FindConfigDir: %s", err.Error())
		}

		for _, fi := range fis {
			if fi.IsDir() && fi.Name() == toFind {
				return path.Join(dirname, fi.Name()), nil
			}
		}
		if dirname == "/" {
			break
		}
		dirname = path.Dir(dirname)
	}
	return "", fmt.Errorf("FindUpDir: unable to find config dir")
}

func FindUpFile(toFind string, dirname string) (string, error) {
	for true {
		fis, err := ioutil.ReadDir(dirname)
		if err != nil {
			return "", fmt.Errorf("FindConfigDir: %s", err.Error())
		}

		for _, fi := range fis {
			if (!fi.IsDir()) && fi.Name() == toFind {
				return path.Join(dirname, fi.Name()), nil
			}
		}
		if dirname == "/" {
			break
		}
		dirname = path.Dir(dirname)
	}
	return "", fmt.Errorf("FindUpDir: unable to find config dir")
}
