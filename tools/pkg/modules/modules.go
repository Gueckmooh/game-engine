package modules

import (
	"encoding/xml"
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"tools/pkg/config"
	"tools/pkg/git"
)

type Module struct {
	XMLName      xml.Name `xml:"module"`
	File         string   `xml:"-"`
	Name         string   `xml:"name,attr"`
	ThirdParty   bool     `xml:"third_party,attr,omitempty"`
	Type         string   `xml:"type"`
	BaseDir      string   `xml:"baseDir"`
	ExportDir    string   `xml:"exportDir"`
	Dependencies []string `xml:"dependencies>dependency,omitempty"`
	Sources      *struct {
		Git *git.GitRepository `xml:"git"`
	} `xml:"sources,omitempty"`
}

type ModuleFileBundle struct {
	basePath string
	files    []string
}

type ModuleBundle struct {
	basePath string
	Modules  []*Module
}

func (mb *ModuleBundle) GetModuleByName(name string) *Module {
	for _, m := range mb.Modules {
		if m.Name == name {
			return m
		}
	}
	return nil
}

func GetModuleFiles(conf *config.Config) (*ModuleFileBundle, error) {
	var filePaths []string
	err := filepath.Walk(conf.ModulesDir, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return fmt.Errorf("walk in %s: %s", conf.ModulesDir, err.Error())
		}

		if info.Mode().IsRegular() {
			if strings.HasSuffix(info.Name(), ".xml") {
				filePaths = append(filePaths, path)
			}
		}

		return nil
	})
	if err != nil {
		return nil, fmt.Errorf("modules.GetModuleFiles: %s", err.Error())
	}

	replacer := strings.NewReplacer(conf.ModulesDir+"/", "")
	for i, fn := range filePaths {
		fn = replacer.Replace(fn)
		filePaths[i] = fn
	}

	modFiles := new(ModuleFileBundle)
	modFiles.basePath = conf.ModulesDir
	modFiles.files = filePaths

	return modFiles, nil
}

func CloneModuleRepository(m *Module) error {
	if m.Sources != nil {
		return m.Sources.Git.Clone()
	} else {
		return fmt.Errorf("Can't clone due to absence of sources in module")
	}
}

func computeDeps(m *Module, mb *ModuleBundle, visited []string, deps map[*Module]bool) error {
	for _, v := range visited {
		if v == m.Name {
			return fmt.Errorf("%s already visited, circular dependencies detected", v)
		}
	}

	deps[m] = true

	for _, dn := range m.Dependencies {
		d := mb.GetModuleByName(dn)
		if d != nil {
			if err := computeDeps(d, mb, append(visited, m.Name), deps); err != nil {
				return err
			}
		}
	}

	return nil
}

func ComputeDependencies(m *Module, mb *ModuleBundle) ([]*Module, error) {
	deps := make(map[*Module]bool)
	err := computeDeps(m, mb, []string{}, deps)
	if err != nil {
		return nil, fmt.Errorf("compute dependencies: %s", err.Error())
	}
	var depL []*Module
	for k, _ := range deps {
		depL = append(depL, k)
	}

	return depL, nil
}
