package main

import (
	"fmt"
	"modules/pkg/config"
	"modules/pkg/modules"
	"modules/pkg/util"
	"os"
	"path"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Printf("usage: %s <xml>\n", os.Args[0])
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

	modules.ExpandModule(mod, conf)

	if mod.Sources.Git != "" {
		err := util.GitClone(mod.Sources.Git, path.Join(conf.SandboxRoot, conf.SrcRoot, mod.BaseDir))
		if err != nil {
			fmt.Printf("main: %s\n", err.Error())
		}
	}

	modules.RunActions(mod.Sources.Actions.Action)

	// 	var mkContent string

	// 	if !genDepsFile {
	// 		mkContent += fmt.Sprintf("MODULE_DIR=%s\n", mod.Name)
	// 		mkContent += fmt.Sprintf("TARGET_KIND=%s\n", mod.Type)
	// 		if mod.Type == "shared_library" {
	// 			mkContent += fmt.Sprintf("TARGET=lib%s\n", strings.Join(strings.Split(mod.Name, "/"), "_"))
	// 			mkContent += fmt.Sprintf("COMPONENT_TYPE=cpp\n")
	// 		} else if mod.Type == "executable" {
	// 			mkContent += fmt.Sprintf("TARGET=%s\n", mod.Name)
	// 			mkContent += fmt.Sprintf("COMPONENT_TYPE=cpp\n")
	// 		} else if mod.Type == "only_headers" {
	// 			mkContent += fmt.Sprintf("COMPONENT_TYPE=headers\n")
	// 		}

	// 		var libdeps []string
	// 		var moddeps []string
	// 		for _, dep := range mod.Dependancies.Dependancy {
	// 			depMod, err := modules.GetModule(dep, conf)
	// 			if err != nil {
	// 				fmt.Printf("main: %s\n", err.Error())
	// 				os.Exit(1)
	// 			}
	// 			if depMod.Type == "shared_library" {
	// 				libdeps = append(libdeps, strings.Join(strings.Split(depMod.Name, "/"), "_"))
	// 			}
	// 			moddeps = append(moddeps, depMod.Name)
	// 		}

	// 		mkContent += fmt.Sprintf("DEPENDANCIES=%s\n", strings.Join(libdeps, " "))
	// 		mkContent += fmt.Sprintf("MODULE_DEPENDANCIES=%s\n", strings.Join(moddeps, " "))

	// 	} else {
	// 		prefix := strings.Replace(mod.Name, "/", "__", -1)
	// 		prefix = strings.ToUpper(prefix)
	// 		prefix += "_"

	// 		mkContent += fmt.Sprintf("%sMODULE_DIR=%s\n", prefix, mod.BaseDir)
	// 		mkContent += fmt.Sprintf("%sMODULE_PATH=$(SRC_DIR)/$(%sMODULE_DIR)\n", prefix, prefix)
	// 		mkContent += fmt.Sprintf("MAKE=sbmake\n")

	// 		var targets []string
	// 		if len(mod.Dependancies.Dependancy) > 0 {
	// 			targets = append(targets, "dependancies")
	// 		}
	// 		if mod.Type != "executable" {
	// 			targets = append(targets, "prebuild")
	// 		}
	// 		if mod.Type != "only_headers" {
	// 			targets = append(targets, "build")
	// 		}

	// 		mkContent += fmt.Sprintf(`.PHONY: %s
	// %s:
	// 	$(QAT)$(MAKE) --no-print-directory -C $(%sMODULE_PATH) %s`, mod.Name, mod.Name, prefix, strings.Join(targets, " "))
	// 	}

	// 	err = ioutil.WriteFile(os.Args[2], []byte(mkContent), 0600)
	// 	if err != nil {
	// 		fmt.Printf("main: %s\n", err.Error())
	// 		os.Exit(1)
	// 	}
}
