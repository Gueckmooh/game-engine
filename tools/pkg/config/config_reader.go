package config

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"
	"path"
)

var configFileName = "config.json"

func SetConfigFileName(filename string) {
	configFileName = filename
}

func ReadConfig(filename string, root string) (*Config, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("config.ReadConfig: %s\n", err.Error())
	}

	data, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, fmt.Errorf("config.ReadConfig: %s\n", err.Error())
	}

	var conf *Config = new(Config)
	err = json.Unmarshal(data, conf)
	if err != nil {
		return nil, fmt.Errorf("config.ReadConfig: %s\n", err.Error())
	}

	conf.SandboxRoot = root

	return conf, nil
}

func GetConfig(root string) (*Config, error) {
	configFile := path.Join(root, configFileName)

	config, err := ReadConfig(configFile, root)
	if err != nil {
		return nil, fmt.Errorf("config.GetConfig: %s\n", err.Error())
	}

	config.SandboxRoot = root
	return config, nil
}
