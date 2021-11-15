package modules

import (
	"encoding/xml"
	"fmt"
	"io/ioutil"
	"modules/pkg/config"
	"modules/pkg/util"
	"os"
	"path"
	"path/filepath"
	"strings"
)

type Action struct {
	Type string `xml:"type,attr"`
	From string `xml:"from,attr"`
	To   string `xml:"to,attr"`
	Text string `xml:",chardata"`
}

type Module struct {
	Name         string `xml:"name,attr"`
	P3           bool   `xml:"p3,attr"`
	Type         string `xml:"type"`
	BaseDir      string `xml:"baseDir"`
	Dependancies struct {
		Dependancy []string `xml:"dependancy"`
	} `xml:"dependancies"`
	Sources struct {
		Git     string `xml:"git"`
		Actions struct {
			Action []Action `xml:"action"`
		} `xml:"actions"`
	} `xml:"sources"`
}

// func RunCopyFile(from string, to string) {
// }

func RunAction(action Action) error {
	switch action.Type {
	case "ExportHeaders":
		files, err := filepath.Glob(action.From)
		if err != nil {
			return fmt.Errorf("RunAction: %s", err.Error())
		}
		for _, v := range files {
			util.ExportHeader(v, path.Join(action.To, path.Base(v)))
		}
	}
	return nil
}

func RunActions(actions []Action) error {
	for _, v := range actions {
		err := RunAction(v)
		if err != nil {
			return fmt.Errorf("RunActions: %s", err.Error())
		}
	}
	return nil
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

func GetModuleByFile(filePath string) (*Module, error) {
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

func expand(s string, mod *Module, conf *config.Config) string {
	s = strings.ReplaceAll(s, "$(src_dir)", path.Join(conf.SrcRoot, mod.BaseDir))
	s = strings.ReplaceAll(s, "$(include_dir)", path.Join(conf.IncludeDir))
	return s
}

func ExpandModule(mod *Module, conf *config.Config) {
	var newAction []Action
	for _, v := range mod.Sources.Actions.Action {
		From := expand(v.From, mod, conf)
		To := expand(v.To, mod, conf)
		Text := expand(v.Text, mod, conf)
		newAction = append(newAction, Action{Type: v.Type, From: From, To: To, Text: Text})
	}
	mod.Sources.Actions.Action = newAction
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
