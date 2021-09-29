package main

import (
	"fmt"
	"modules/pkg/build"
	"modules/pkg/colors"
	"modules/pkg/config"
	"modules/pkg/make"
	"modules/pkg/modules"
	"os"
	"path"
	"path/filepath"
	"regexp"
)

func getModDir(conf *config.Config, cwd string) (string, error) {
	moddir, err := filepath.Rel(path.Join(conf.SandboxRoot, conf.SrcRoot), cwd)
	if err != nil {
		return "", fmt.Errorf("getModDir: %s\n", err.Error())
	}
	return moddir, nil
}

type Opts struct {
	Cwd           string
	MTB           string
	BuildUpstream bool
}

func parseArgs(Args []string) (Opts, []string) {
	var opts Opts
	var optl []string
	args := Args[1:]
	shift := false
	mtbRe := regexp.MustCompile(`([^=]*)=(.*)`)
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
		case "-build-upstream":
			opts.BuildUpstream = true
			shift = true
		default:
			m := mtbRe.FindStringSubmatch(o)
			if len(m) > 0 {
				switch m[1] {
				case "MTB":
					opts.MTB = m[2]
					optl = append(optl, fmt.Sprintf("%s=%s", "MODULE_TO_BUILD", m[2]))
				default:
					if !shift {
						optl = append(optl, o)
					} else {
						shift = false
					}
				}
			} else {
				if !shift {
					optl = append(optl, o)
				} else {
					shift = false
				}
			}
		}
	}
	return opts, optl
}

func checkOpts(opts Opts) error {
	if opts.Cwd != "" && opts.MTB != "" {
		return fmt.Errorf("options -C and MTB=... are mutually exclusive")
	}
	return nil
}

func runSBMake(opts Opts, optl []string) error {
	var cwd string
	var err error
	if opts.Cwd == "" {
		cwd, err = os.Getwd()
		if err != nil {
			return fmt.Errorf("runSBMake: %s", err.Error())
		}
	} else {
		cwd, err = filepath.Abs(opts.Cwd)
		if err != nil {
			return fmt.Errorf("runSBMake: %s", err.Error())
		}
	}

	conf, err := config.GetConfig(cwd)
	if err != nil {
		return fmt.Errorf("runSBMake: %s", err.Error())
	}

	vars, err := make.ParseMakefile(path.Join(cwd, "Makefile"))
	if err != nil {
		return fmt.Errorf("runSBMake: %s", err.Error())
	}

	var currentMod string
	if opts.MTB != "" {
		currentMod = opts.MTB
	} else if value, ok := vars["MODULE"]; ok {
		currentMod = value
	} else {
		return fmt.Errorf("mail: MODULE should be defined\n")
	}

	mod, err := modules.GetModule(currentMod, conf)
	if err != nil {
		return fmt.Errorf("runSBMake: %s", err.Error())
	}

	if opts.MTB != "" {
		cwd = path.Join(conf.SandboxRoot, conf.SrcRoot, mod.BaseDir)
		optl = append(optl, "-C", cwd)
	}

	err = build.PrepareBuildArea(conf)
	if err != nil {
		return fmt.Errorf("runSBMake: %s", err.Error())
	}

	if opts.BuildUpstream {
		deps, err := modules.ComputeDependancies(mod, conf)
		if err != nil {
			return fmt.Errorf("runSBMake: %s", err.Error())
		}

		err = forDepth(deps, func(curmod *modules.Module) error {
			if curmod.Name != mod.Name {
				return runSBMake(Opts{MTB: curmod.Name}, optl)
			}
			return nil
		})
		if err != nil {
			return fmt.Errorf("runSBMake: %s", err.Error())
		}
	}

	fmt.Printf("%sruning sbmake for module '%s%s%s'%s\n", colors.ColorBlue, colors.ColorRed, mod.Name, colors.ColorBlue, colors.ColorReset)

	err = build.CopyIncludeFiles(conf, mod)
	if err != nil {
		return fmt.Errorf("runSBMake: %s", err.Error())
	}

	if mod.Type != "only_headers" {
		err = make.RunMake(conf, mod, optl)
		if err != nil {
			return fmt.Errorf("runSBMake: %s", err.Error())
		}
	}
	return nil
}

func forDepth(node *modules.DepTree, f func(*modules.Module) error) error {
	for _, succ := range node.Deps {
		err := forDepth(succ, f)
		if err != nil {
			return err
		}
	}
	return f(node.Mod)
}

func main() {
	opts, optl := parseArgs(os.Args)

	if err := checkOpts(opts); err != nil {
		fmt.Printf("main: %s\n", err.Error())
		os.Exit(1)
	}

	if err := runSBMake(opts, optl); err != nil {
		fmt.Printf("main: %s\n", err.Error())
		os.Exit(1)
	}
}
