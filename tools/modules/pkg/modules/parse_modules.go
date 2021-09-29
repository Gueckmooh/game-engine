package modules

import (
	"encoding/xml"
	"fmt"
	"io/ioutil"
	"modules/pkg/config"
	"os"
	"path"
)

type Module struct {
	Name         string `xml:"name,attr"`
	Type         string `xml:"type"`
	BaseDir      string `xml:"baseDir"`
	Dependancies struct {
		Dependancy []string `xml:"dependancy"`
	} `xml:"dependancies"`
}

const (
	defaultModulesDir = "modules"
)

var modulesDir = defaultModulesDir

func parseModule(filePath string) (*Module, error) {
	_, err := os.Stat(filePath)
	if os.IsNotExist(err) {
		return nil, fmt.Errorf("modules::parseModule: %s", err.Error())
	}

	file, err := os.Open(filePath)
	if err != nil {
		return nil, fmt.Errorf("modules::parseModule: %s", err.Error())
	}

	data, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, fmt.Errorf("modules::parseModule: %s", err.Error())
	}

	m := &Module{}
	err = xml.Unmarshal(data, m)
	if err != nil {
		return nil, fmt.Errorf("modules::parseModule: %s", err.Error())
	}

	return m, nil
}

func checkModuleIntegrity(mod *Module) error {
	// @todo
	return nil
}

func GetModule(moduleName string, conf *config.Config) (*Module, error) {
	modFileName := fmt.Sprintf("%s.%s", moduleName, "xml")
	filePath := path.Join(conf.SandboxRoot, conf.ModulesDir, modFileName)
	mod, err := parseModule(filePath)
	if err != nil {
		return nil, err
	}

	err = checkModuleIntegrity(mod)
	if err != nil {
		return nil, err
	}

	return mod, nil
}

type DepTree struct {
	Mod  *Module
	Deps []*DepTree
}

func ComputeDependancies(mod *Module, conf *config.Config) (*DepTree, error) {
	var root *DepTree = new(DepTree)
	var visited map[string]bool = make(map[string]bool)

	root.Mod = mod
	var computeRec func(*DepTree, *Module) error
	computeRec = func(node *DepTree, mod *Module) error {
		if _, v := visited[mod.Name]; v {
			return nil
		}
		visited[mod.Name] = true
		for _, dep := range mod.Dependancies.Dependancy {
			depMod, err := GetModule(dep, conf)
			if err != nil {
				return fmt.Errorf("computeRec: %s", err.Error())
			}
			nextNode := &DepTree{Mod: depMod}
			err = computeRec(nextNode, depMod)
			if err != nil {
				return fmt.Errorf("computeRec: %s", err.Error())
			}
			node.Deps = append(node.Deps, nextNode)
		}
		return nil
	}

	err := computeRec(root, mod)
	if err != nil {
		return nil, fmt.Errorf("ComputeDependancies: %s", err.Error())
	}
	return root, nil
}
