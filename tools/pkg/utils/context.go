package utils

import (
	"regexp"
	"strings"
)

type Context struct {
	values map[string]string
}

func MakeContext() *Context {
	var c *Context = &Context{}
	c.values = make(map[string]string)
	return c
}

func (c *Context) SetValue(name string, value string) {
	c.values[name] = value
}

func (c *Context) GetValue(name string) (string, bool) {
	value, ok := c.values[name]
	return value, ok
}

var re = regexp.MustCompile(`\$\(([^)]*)\)`)

func CanExpand(value string) bool {
	return len(re.FindAllStringSubmatch(value, -1)) > 0
}

func (c *Context) Expand(value string) (string, bool) {
	m := re.FindAllStringSubmatch(value, -1)
	for _, m := range m {
		n := m[1]
		if v, ok := c.GetValue(n); ok {
			value = strings.ReplaceAll(value, "$("+n+")", v)
		}
	}
	return value, (len(re.FindString(value)) == 0)
}
