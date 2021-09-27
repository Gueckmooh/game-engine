package main

import (
	"flag"
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
	flag.StringVar(&opts.Cwd, "C", "", "")
	flag.Parse()
	return opts, os.Args[1:]
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

	currentMod, err := getModDir(conf, cwd)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
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

	err = make.RunMake(conf, mod, optl)
	if err != nil {
		fmt.Printf("main: %s\n", err.Error())
		return
	}
}
