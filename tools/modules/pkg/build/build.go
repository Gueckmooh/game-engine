package build

import (
	"bytes"
	"fmt"
	"io"
	"io/ioutil"
	"modules/pkg/config"
	"modules/pkg/modules"
	"os"
	"os/exec"
	"path"
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

func CopyIncludeFiles(conf *config.Config, mod *modules.Module) error {
	if mod.Type == "executable" {
		return nil
	}
	includeModPath := path.Join(conf.SandboxRoot, conf.SrcRoot, mod.BaseDir, includeDir)
	includePath := path.Join(conf.SandboxRoot, conf.BuildDir, includeDir)

	files, err := ioutil.ReadDir(includeModPath)
	if err != nil {
		return fmt.Errorf("CopyIncludeFiles: Copying images: %s", err.Error())
	}

	var stdBuffer bytes.Buffer
	mw := io.MultiWriter(os.Stdout, &stdBuffer)

	for _, file := range files {
		filename := path.Join(includeModPath, file.Name())
		cmd := exec.Command("cp", "-r", filename, includePath)
		cmd.Stdout = mw
		cmd.Stderr = mw

		if err := cmd.Run(); err != nil {
			return fmt.Errorf("CopyIncludeFiles: Copying images: %s", err.Error())
		}
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
