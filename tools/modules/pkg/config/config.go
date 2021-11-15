package config

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"modules/pkg/util"
	"os"
	"path"
	"strings"
)

type Config struct {
	SandboxRoot string
	SrcRoot     string `json:"source_root"`
	BuildDir    string `json:"build_dir"`
	ModulesDir  string `json:"modules_dir"`
	MakeDir     string `json:"makes_dir"`
	ScriptsDir  string `json:"scripts_dir"`
	IncludeDir  string `json:"include_dir"`
}

var configFileName = "config.json"

func expand(s string, conf *Config) string {
	s = strings.ReplaceAll(s, "$(source_root)", conf.SrcRoot)
	s = strings.ReplaceAll(s, "$(build_dir)", conf.BuildDir)
	s = strings.ReplaceAll(s, "$(modules_dir)", conf.ModulesDir)
	s = strings.ReplaceAll(s, "$(makes_dir)", conf.MakeDir)
	s = strings.ReplaceAll(s, "$(scripts_dir)", conf.MakeDir)
	s = strings.ReplaceAll(s, "$(include_dir)", conf.IncludeDir)
	return s
}

func expandConfig(conf *Config) {
	conf.SrcRoot = expand(conf.SrcRoot, conf)
	conf.BuildDir = expand(conf.BuildDir, conf)
	conf.ModulesDir = expand(conf.ModulesDir, conf)
	conf.MakeDir = expand(conf.MakeDir, conf)
	conf.ScriptsDir = expand(conf.ScriptsDir, conf)
	conf.IncludeDir = expand(conf.IncludeDir, conf)
}

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
	expandConfig(conf)
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
