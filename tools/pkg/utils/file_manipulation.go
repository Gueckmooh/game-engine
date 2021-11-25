package utils

import (
	"fmt"
	"io"
	"os"
	"path"
)

func Mkdir(path string) error {
	_, err := os.Stat(path)
	if os.IsNotExist(err) {
		err = os.MkdirAll(path, 0755)
		if err != nil {
			return fmt.Errorf("utils.Mkdir: %s", err.Error())
		}
	}
	return nil
}

func fileOrDirExists(path string) bool {
	_, err := os.Stat(path)
	if os.IsNotExist(err) {
		return false
	}
	return true
}

func isDir(path string) bool {
	stat, err := os.Stat(path)
	if err != nil {
		return false // ignore error
	}
	return stat.IsDir()
}

func DirExists(path string) bool {
	return fileOrDirExists(path) && isDir(path)
}

func FileExists(path string) bool {
	return fileOrDirExists(path) && (!isDir(path))
}

func CopyFile(src string, dst string) error {
	if !DirExists(path.Dir(dst)) {
		Mkdir(path.Dir(dst))
	}

	srcStat, err := os.Stat(src)
	if err != nil {
		return fmt.Errorf("utils.CopyFile: %s", err.Error())
	}

	if !srcStat.Mode().IsRegular() {
		return fmt.Errorf("utils.CopyFile: file '%s' is not regular", src)
	}

	srcFile, err := os.Open(src)
	if err != nil {
		return fmt.Errorf("utils.CopyFile: %s", err.Error())
	}
	defer srcFile.Close()

	dstFile, err := os.Create(dst)
	if err != nil {
		return fmt.Errorf("utils.CopyFile: %s", err.Error())
	}
	defer dstFile.Close()

	_, err = io.Copy(dstFile, srcFile)
	if err != nil {
		return fmt.Errorf("utils.CopyFile: %s", err.Error())
	}
	return nil
}
