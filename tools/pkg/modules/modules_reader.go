package modules

import (
	"encoding/xml"
	"fmt"
	"io/ioutil"
	"os"
	"path"
)

func ReadModule(filename string) (*Module, error) {
	_, err := os.Stat(filename)
	if os.IsNotExist(err) {
		return nil, fmt.Errorf("module.ReadModule: reading file %s: %s\n", filename, err.Error())
	}

	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("module.ReadModule: reading file %s: %s\n", filename, err.Error())
	}

	data, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, fmt.Errorf("module.ReadModule: reading file %s: %s\n", filename, err.Error())
	}

	var conf *Module = new(Module)
	err = xml.Unmarshal(data, conf)
	if err != nil {
		return nil, fmt.Errorf("module.ReadModule: reading file %s: %s\n", filename, err.Error())
	}

	return conf, nil
}

func ReadModuleBundle(bundle *ModuleFileBundle) (*ModuleBundle, error) {
	var moduleList []*Module
	for _, file := range bundle.files {
		filePath := path.Join(bundle.basePath, file)
		mod, err := ReadModule(filePath)
		if err != nil {
			return nil, fmt.Errorf("module.ReadModuleBundle: %s", err.Error())
		}

		mod.File = file
		moduleList = append(moduleList, mod)
	}

	modBundle := new(ModuleBundle)

	modBundle.basePath = bundle.basePath
	modBundle.Modules = moduleList

	return modBundle, nil
}
