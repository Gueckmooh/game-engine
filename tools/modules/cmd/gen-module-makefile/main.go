package main

import (
	"fmt"
	"io/ioutil"
	"modules/pkg/config"
	"modules/pkg/modules"
	"os"
	"strings"
)

func main() {
	if len(os.Args) < 3 {
		fmt.Printf("usage: %s <xml> <mk>\n", os.Args[0])
		os.Exit(1)
	}

	cwd, err := os.Getwd()
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		os.Exit(1)
	}

	conf, err := config.GetConfig(cwd)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		os.Exit(1)
	}

	mod, err := modules.GetModuleByFile(os.Args[1])
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		os.Exit(1)
	}

	var mkContent string

	mkContent += fmt.Sprintf("MODULE_DIR=%s\n", mod.Name)
	mkContent += fmt.Sprintf("TARGET_KIND=%s\n", mod.Type)
	if mod.Type == "shared_library" {
		mkContent += fmt.Sprintf("TARGET=lib%s\n", mod.Name)
		mkContent += fmt.Sprintf("COMPONENT_TYPE=cpp\n")
	} else if mod.Type == "executable" {
		mkContent += fmt.Sprintf("TARGET=%s\n", mod.Name)
		mkContent += fmt.Sprintf("COMPONENT_TYPE=cpp\n")
	} else if mod.Type == "only_headers" {
		mkContent += fmt.Sprintf("COMPONENT_TYPE=headers\n")
	}

	var libdeps []string
	for _, dep := range mod.Dependancies.Dependancy {
		depMod, err := modules.GetModule(dep, conf)
		if err != nil {
			fmt.Printf("main: %s\n", err.Error())
			os.Exit(1)
		}
		if depMod.Type == "shared_library" {
			libdeps = append(libdeps, depMod.Name)
		}
	}

	mkContent += fmt.Sprintf("DEPENDANCIES=%s\n", strings.Join(libdeps, " "))

	err = ioutil.WriteFile(os.Args[2], []byte(mkContent), 0600)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		os.Exit(1)
	}
}
