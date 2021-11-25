package utils

func IsBool(v interface{}) bool {
	switch v.(type) {
	case bool:
		return true
	default:
		return false
	}
}

func IsString(v interface{}) bool {
	switch v.(type) {
	case string:
		return true
	default:
		return false
	}
}
