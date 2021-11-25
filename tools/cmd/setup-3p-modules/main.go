package main

import (
	"fmt"
	"os"
	"path"
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
	for _, m := range modBundle.Modules {
		if m.Sources != nil && m.Sources.Git != nil {
			m.Sources.Git.Target = path.Join(conf.SrcDir, m.BaseDir, m.Sources.Git.Target)
		}
	}

	fmt.Println("Generating 3p module makefiles...")
	err = build.GenrateModuleMakefileBundle(newConfig, modBundle, true)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	fmt.Println("Generating 3p module config makefiles...")
	err = build.GenerateModuleBundleConfigMakefile(newConfig, modBundle, true)
	if err != nil {
		return fmt.Errorf("tryMain: %s", err.Error())
	}

	fmt.Println("Cloning 3p modules...")
	for _, m := range modBundle.Modules {
		if m.ThirdParty && m.Sources != nil && m.Sources.Git != nil {
			err = modules.CloneModuleRepository(m)
			if err != nil {
				return fmt.Errorf("tryMain: %s", err.Error())
			}
		}
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
