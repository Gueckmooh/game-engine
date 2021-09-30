package make

import (
	"bytes"
	"fmt"
	"io"
	"modules/pkg/config"
	"modules/pkg/modules"
	"os"
	"os/exec"
	"path"
	"strings"
)

func nameToBinName(name string) string {
	return strings.Replace(name, "/", "_", -1)
}

func makeLibDeps(conf *config.Config, mod *modules.Module) (string, error) {
	var deps []string
	for _, dep := range mod.Dependancies.Dependancy {
		m, err := modules.GetModule(dep, conf)
		if err != nil {
			return "", fmt.Errorf("makeLibDeps: %s", err.Error())
		}
		if m.Type != "only_headers" {
			deps = append(deps, nameToBinName(m.Name))
		}
	}
	if len(deps) > 0 {
		return fmt.Sprintf("DEPLIBS=%s", strings.Join(deps, ",")), nil
	} else {
		return "", nil
	}
}

func makeParams(conf *config.Config, mod *modules.Module) ([]string, error) {
	var params []string
	params = append(params, fmt.Sprintf("BASE=%s", path.Join(conf.SandboxRoot)))
	params = append(params, fmt.Sprintf("MAKEDIR=%s", path.Join(conf.SandboxRoot, conf.MakeDir)))
	params = append(params, fmt.Sprintf("MODDIR=%s", path.Join(mod.BaseDir)))

	if mod.Type == "dynamic_lib" {
		params = append(params, "DYNAMIC_LIB=1")
		params = append(params, fmt.Sprintf("BINNAME=lib%s", nameToBinName(mod.Name)))
	} else if mod.Type == "executable" {
		params = append(params, "EXECUTABLE=1")
		params = append(params, fmt.Sprintf("BINNAME=%s", nameToBinName(mod.Name)))
	}

	libDep, err := makeLibDeps(conf, mod)
	if err != nil {
		return nil, fmt.Errorf("makeParams: %s", err.Error())
	}
	if libDep != "" {
		params = append(params, libDep)
	}

	return params, nil
}

func RunMake(conf *config.Config, mod *modules.Module, optl []string) error {
	params, err := makeParams(conf, mod)
	params = append(params, optl...)
	if err != nil {
		return fmt.Errorf("RunMake: %s", err.Error())
	}

	cmd := exec.Command("make", params...)

	fmt.Printf("sbmake: %s %s\n", "make", strings.Join(params, " "))

	var stdBuffer bytes.Buffer
	mw := io.MultiWriter(os.Stdout, &stdBuffer)

	cmd.Stdout = mw
	cmd.Stderr = mw

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("RunMake: Copying images: %s", err.Error())
	}
	return nil
}
