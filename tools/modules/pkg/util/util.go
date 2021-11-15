package util

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"path"
	"path/filepath"
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

func TryMkdir(path string) error {
	_, err := os.Stat(path)
	if os.IsNotExist(err) {
		err = os.MkdirAll(path, 0755)
		if err != nil {
			return fmt.Errorf("tryMkdir: %s", err.Error())
		}
	}
	return nil
}

func GitClone(repo string, target string) error {
	err := TryMkdir(path.Dir(target))
	if err != nil {
		return fmt.Errorf("GitClone: %s", err.Error())
	}
	cmd := exec.Command("git", "clone", repo, target)
	fmt.Printf("git clone %s %s\n", repo, target)

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("GitClone: %s", err.Error())
	}
	return nil
}

func makeHeaderContent(p1 string, p2 string) (string, error) {
	relpath, err := filepath.Rel(path.Dir(p1), p2)
	if err != nil {
		return "", err
	}

	content := fmt.Sprintf(`#pragma once

#include "%s"`, relpath)

	return content, nil
}

func ExportHeader(toExport string, target string) error {
	err := TryMkdir(path.Dir(target))
	if err != nil {
		return fmt.Errorf("ExportError: %s", err.Error())
	}

	str, err := makeHeaderContent(target, toExport)
	if err != nil {
		return fmt.Errorf("ExportError: %s", err.Error())
	}

	err = ioutil.WriteFile(target, []byte(str), 0600)
	if err != nil {
		return err
	}

	return nil
}
