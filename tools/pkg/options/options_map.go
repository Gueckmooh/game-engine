package options

import "tools/pkg/utils"

const (
	defaultBool   = false
	defaultString = ""
)

var optionMap map[string]interface{} = nil

func initMap() {
	if optionMap == nil {
		optionMap = make(map[string]interface{})
	}
}

func instance() map[string]interface{} {
	if optionMap == nil {
		initMap()
	}
	return optionMap
}

func SetOption(name string, value interface{}) {
	m := instance()
	m[name] = value
}

func GetOption(name string) interface{} {
	m := instance()
	value, ok := m[name]
	if ok {
		return value
	} else {
		return nil
	}
}

func GetOptionBool(name string) bool {
	v := GetOption(name)
	if v != nil && utils.IsBool(v) {
		return v.(bool)
	}
	return defaultBool
}

func GetOptionString(name string) string {
	v := GetOption(name)
	if v != nil && utils.IsString(v) {
		return v.(string)
	}
	return defaultString
}
