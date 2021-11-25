package main

import (
	"fmt"
	"os"
	"tools/pkg/build"
	"tools/pkg/config"
	"tools/pkg/modules"
	"tools/pkg/options"
	"tools/pkg/utils"
)

func processOptions() {
	for _, option := range os.Args {
		switch option {
		case "--allow-overwrite":
			options.SetOption("overwrite-config-file", true)
		case "--static-libraries":
			options.SetOption("library-kind", "static")
		case "--dynamic-libraries":
			options.SetOption("library-kind", "dynamic")
		}
	}
}

func tryMain() error {
	cwd, err := os.Getwd()
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	fmt.Println("Reading config file...")
	conf, err := config.GetConfig(cwd)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	c := utils.MakeContext()
	newConfig := conf.Expand(c)

	modFiles, err := modules.GetModuleFiles(newConfig)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	fmt.Println("Reading module files...")
	modBundle, err := modules.ReadModuleBundle(modFiles)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	fmt.Println("Generating module makefiles...")
	err = build.GenrateModuleMakefileBundle(newConfig, modBundle, false)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	fmt.Println("Generating module config makefiles...")
	err = build.GenerateModuleBundleConfigMakefile(newConfig, modBundle, false)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	fmt.Println("Generating makerule config file...")
	err = build.GenerateConfigMakefile(newConfig, conf)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	return nil
}

func main() {
	processOptions()
	err := tryMain()
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		os.Exit(1)
	}
}
