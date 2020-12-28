package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/exec"
	"runtime"
	"strings"

	"github.com/gin-gonic/gin"
	"github.com/go-cmd/cmd"
)

type RunCodeParams struct {
	Codeinput    string `json:"codeinput"`
	Logging      string `json:"logging"`
	LoggingLevel string `json:"loggingLevel"`
	Stdin        string `json:"stdin"`
}

func splitstring(s string) string {
	s = strings.Replace(s, "\t", "&nbsp;&nbsp;&nbsp;&nbsp;", -1)
	s = strings.Replace(s, " ", "&nbsp;", -1)
	s = strings.Replace(s, "\n", "</br>", -1)
	return s
}
func RunCodeHandler(c *gin.Context) {
	//logging_levels := [5]string{"fatal", "error", "info", "debug", "warn"}
	var JSONInput RunCodeParams
	if err := c.ShouldBindJSON(&JSONInput); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{
			"success": false,
			"data":    "无效输入" + err.Error(),
		})
		return
	}
	//源码
	fd, _ := os.OpenFile("../compiler/testfile.txt", os.O_WRONLY|os.O_CREATE, 0644)
	defer fd.Close()
	fmt.Fprintf(fd, "%s", JSONInput.Codeinput)
	//stdin
	fd, _ = os.OpenFile("../compiler/stdin.txt", os.O_WRONLY|os.O_CREATE, 0644)
	defer fd.Close()
	fmt.Fprintf(fd, "%s", JSONInput.Stdin)
	//exec compile
	os.Chdir("../compiler")
	var res *exec.Cmd
	if JSONInput.Logging == "on" {
		res = exec.Command("compiler.exe", "testfile.txt", "-o", "out.txt", "-l", JSONInput.LoggingLevel)
	} else {
		res = exec.Command("compiler.exe", "testfile.txt", "-o", "out.txt")
	}
	_, err := res.Output()
	if err != nil {
		panic(err)
	}
	fd, _ = os.Open("../compiler/log.txt")
	defer fd.Close()
	log_bytes, _ := ioutil.ReadAll(fd)
	log := splitstring(string(log_bytes))
	if strings.Contains(log, "error:&nbsp;") {
		c.JSON(http.StatusOK, gin.H{
			"success": false,
			"data":    "编译失败",
			"log":     log,
		})
		return
	}
	fd, _ = os.Open("../compiler/out.txt")
	defer fd.Close()
	pcode_bytes, _ := ioutil.ReadAll(fd)
	pcode := splitstring(string(pcode_bytes))
	cout := cmd.NewCmd("pcode_interpreter.exe", "<", "stdin.txt")
	<-cout.Start()
	fmt.Println(cout.Status().Stdout)

	var output string
	for _, v := range cout.Status().Stdout {
		output += v + "</br>"
	}

	c.JSON(http.StatusOK, gin.H{
		"success": true,
		"data":    "运行成功",
		"log":     log,
		"pcode":   pcode,
		"output":  output,
	})
	//chdir back
	os.Chdir("../webservice")
	return
}
func openbrowser(url string) {
	var err error

	switch runtime.GOOS {
	case "linux":
		err = exec.Command("xdg-open", url).Start()
	case "windows":
		err = exec.Command("rundll32", "url.dll,FileProtocolHandler", url).Start()
	case "darwin":
		err = exec.Command("open", url).Start()
	default:
		err = fmt.Errorf("unsupported platform")
	}
	if err != nil {
		log.Fatal(err)
	}

}
func main() {
	g := gin.Default()
	g.LoadHTMLGlob("templates/*")
	g.GET("/", func(c *gin.Context) {
		c.HTML(http.StatusOK, "index.html", nil)
	})
	g.POST("/", RunCodeHandler)
	go openbrowser("http://127.0.0.1:8810")
	g.Run("0.0.0.0:8810")
}
