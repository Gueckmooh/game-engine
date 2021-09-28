package main

import (
	"fmt"
	"modules/pkg/build"
	"modules/pkg/config"
	"modules/pkg/make"
	"modules/pkg/modules"
	"os"
	"path"
	"path/filepath"
)

func getModDir(conf *config.Config, cwd string) (string, error) {
	moddir, err := filepath.Rel(path.Join(conf.SandboxRoot, conf.SrcRoot), cwd)
	if err != nil {
		return "", fmt.Errorf("getModDir: %s\n", err.Error())
	}
	return moddir, nil
}

type Opts struct {
	Cwd string
}

func parseArgs() (Opts, []string) {
	var opts Opts
	var optl []string
	args := os.Args[1:]
	shift := false
	for i, o := range args {
		switch o {
		case "-C":
			if len(args) > i+1 {
				opts.Cwd = args[i+1]
			} else {
				fmt.Printf("-C option needs an argument\n")
			}
			optl = append(optl, o, opts.Cwd)
			shift = true
		default:
			if !shift {
				optl = append(optl, o)
			} else {
				shift = false
			}
		}
	}
	return opts, optl
}

func main() {
	opts, optl := parseArgs()
	var cwd string
	var err error
	if opts.Cwd == "" {
		cwd, err = os.Getwd()
		if err != nil {
			fmt.Printf("main: %s\n", err.Error())
			return
		}
	} else {
		cwd, err = filepath.Abs(opts.Cwd)
		if err != nil {
			fmt.Printf("main: %s\n", err.Error())
			return
		}
	}

	conf, err := config.GetConfig(cwd)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		return
	}

	// currentMod, err := getModDir(conf, cwd)
	// if err != nil {
	// 	fmt.Printf("main: %s\n", err.Error())
	// 	return
	// }

	vars, err := make.ParseMakefile(path.Join(cwd, "Makefile"))
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		return
	}

	var currentMod string
	if value, ok := vars["MODULE"]; ok {
		currentMod = value
	} else {
		fmt.Printf("mail: MODULE should be defined\n")
		return
	}

	mod, err := modules.GetModule(currentMod, conf)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		return
	}

	err = build.PrepareBuildArea(conf)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		return
	}

	err = build.CopyIncludeFiles(conf, mod)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		return
	}

	if mod.Type != "only_headers" {
		err = make.RunMake(conf, mod, optl)
		if err != nil {
			fmt.Printf("main: %s\n", err.Error())
			return
		}
	}
}
