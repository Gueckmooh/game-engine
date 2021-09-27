package config

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"modules/pkg/util"
	"os"
	"path"
)

type Config struct {
	SandboxRoot string
	SrcRoot     string `json:"source_root"`
	BuildDir    string `json:"build_dir"`
	ModulesDir  string `json:"modules_dir"`
	MakeDir     string `json:"makes_dir"`
}

var configFileName = "config.json"

func ReadConfig(filename string) (*Config, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("readConfig: %s\n", err.Error())
	}

	data, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, fmt.Errorf("readConfig: %s\n", err.Error())
	}

	var conf *Config = new(Config)
	err = json.Unmarshal(data, conf)
	if err != nil {
		return nil, fmt.Errorf("readConfig: %s\n", err.Error())
	}
	return conf, nil
}

func GetConfig(cwd string) (*Config, error) {
	configFile, err := util.FindUpFile(configFileName, cwd)
	if err != nil {
		return nil, fmt.Errorf("GetConfig: %s\n", err.Error())
	}

	config, err := ReadConfig(configFile)
	if err != nil {
		return nil, fmt.Errorf("GetConfig: %s\n", err.Error())
	}

	config.SandboxRoot = path.Dir(configFile)
	return config, nil
}
