package git

import (
	"bytes"
	"fmt"
	"io"
	"os"
	"os/exec"
	"path"
	"tools/pkg/options"
	"tools/pkg/utils"
)

type GitRepository struct {
	Url    string `xml:",chardata"`
	Target string `xml:"target,attr"`
	Commit string `xml:"commit,attr"`
}

func CloneRepository(url string, target string) (error, bool) {
	if !utils.DirExists(path.Dir(target)) {
		if err := utils.Mkdir(path.Dir(target)); err != nil {
			return fmt.Errorf("git.CloneRepository: %s", err.Error()), false
		}
	}

	cmd := exec.Command("git", "clone", url, target)

	fmt.Printf("Cloning git repository \"%s\" into \"%s\"\n", url, target)

	var stdBuffer bytes.Buffer
	if options.GetOptionBool("git-verbose") {

		mw := io.MultiWriter(os.Stdout, &stdBuffer)

		cmd.Stdout = mw
		cmd.Stderr = mw
	}

	if utils.DirExists(target) {
		fmt.Println("Ignored because dir already exists.")
		return nil, false
	}

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("git.CloneRepository: %s", err.Error()), false
	}

	return nil, true
}

func CheckoutRepository(target, commit string) error {
	cwd, err := os.Getwd()
	if err != nil {
		return fmt.Errorf("CheckoutRepository: %s", err.Error())
	}

	err = os.Chdir(target)
	if err != nil {
		return fmt.Errorf("CheckoutRepository: %s", err.Error())
	}

	cmd := exec.Command("git", "checkout", commit)

	fmt.Printf("Checkout commit \"%s\" for \"%s\"\n", commit, target)

	var stdBuffer bytes.Buffer
	if options.GetOptionBool("git-verbose") {

		mw := io.MultiWriter(os.Stdout, &stdBuffer)

		cmd.Stdout = mw
		cmd.Stderr = mw
	}

	if err := cmd.Run(); err != nil {
		return fmt.Errorf("CheckoutRepository: %s", err.Error())
	}

	err = os.Chdir(cwd)
	if err != nil {
		return fmt.Errorf("CheckoutRepository: %s", err.Error())
	}
	return nil
}

func (g *GitRepository) Clone() error {
	err, ok := CloneRepository(g.Url, g.Target)
	if err != nil {
		return err
	}
	if ok && g.Commit != "" {
		return CheckoutRepository(g.Target, g.Commit)
	}
	return nil
}
