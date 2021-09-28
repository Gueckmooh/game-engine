package make

import (
	"fmt"
	"io/ioutil"
	"os"
	"regexp"
	"strings"
)

var makeVarRe = regexp.MustCompile(`([^=]*)=(.*)`)

func ParseMakefile(filename string) (map[string]string, error) {
	var err error
	var variables map[string]string = make(map[string]string)
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("ParseMakefile: %s", err.Error())
	}

	data, err := ioutil.ReadAll(file)
	if err != nil {
		return nil, fmt.Errorf("ParseMakefile: %s", err.Error())
	}

	content := string(data)
	lines := strings.Split(content, "\n")
	for _, line := range lines {
		m := makeVarRe.FindStringSubmatch(line)
		if len(m) > 0 {
			variables[m[1]] = m[2]
		}
	}

	return variables, nil
}
