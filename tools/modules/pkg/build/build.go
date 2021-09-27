package build

import (
	"fmt"
	"io/ioutil"
	"modules/pkg/config"
	"modules/pkg/modules"
	"os"
	"path"
	"path/filepath"
)

const includeDir = "include"

func tryMkdir(path string) error {
	_, err := os.Stat(path)
	if os.IsNotExist(err) {
		err = os.Mkdir(path, 0755)
		if err != nil {
			return fmt.Errorf("tryMkdir: %s", err.Error())
		}
	}
	return nil
}

func makeHeaderContent(p1 string, p2 string) (string, error) {
	relpath, err := filepath.Rel(p1, p2)
	if err != nil {
		return "", err
	}

	content := fmt.Sprintf(`#pragma once

#include "%s"`, relpath)

	return content, nil
}

func CopyIncludeFiles(conf *config.Config, mod *modules.Module) error {
	if mod.Type == "executable" {
		return nil
	}
	includeModPath := path.Join(conf.SandboxRoot, conf.SrcRoot, mod.BaseDir, includeDir)
	includePath := path.Join(conf.SandboxRoot, conf.BuildDir, includeDir)

	err := filepath.Walk(includeModPath, func(p string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}

		fmt.Println(p, info.IsDir())
		relPath, err := filepath.Rel(includeModPath, p)
		if err != nil {
			return err
		}
		if info.IsDir() && relPath != "." {
			tryMkdir(path.Join(includePath, relPath))
		} else if !info.IsDir() {
			targetFile := path.Join(includePath, relPath)
			content, err := makeHeaderContent(targetFile, p)
			if err != nil {
				return err
			}

			err = ioutil.WriteFile(targetFile, []byte(content), 0600)
			if err != nil {
				return err
			}
		}

		return nil
	})
	if err != nil {
		return fmt.Errorf("CopyIncludeFiles: Copying images: %s", err.Error())
	}

	return nil
}

func PrepareBuildArea(conf *config.Config) error {
	buildDir := path.Join(conf.SandboxRoot, conf.BuildDir)
	err := tryMkdir(buildDir)
	if err != nil {
		return fmt.Errorf("PrepareBuildArea: %s", err.Error())
	}

	err = tryMkdir(path.Join(buildDir, "lib"))
	if err != nil {
		return fmt.Errorf("PrepareBuildArea: %s", err.Error())
	}

	err = tryMkdir(path.Join(buildDir, "bin"))
	if err != nil {
		return fmt.Errorf("PrepareBuildArea: %s", err.Error())
	}

	err = tryMkdir(path.Join(buildDir, "include"))
	if err != nil {
		return fmt.Errorf("PrepareBuildArea: %s", err.Error())
	}

	return nil
}
