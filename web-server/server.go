package main

// import related packages
import (
	"encoding/json"
	"fmt"
	"io"
	"log"
	"net/http"
	"os"
	"os/exec"
	"path/filepath"
	"strings"
)


// define struct for getting code requests
type RunRequest struct {
	Code string `json:"code"`
}

// define struct for getting code response compilation
type RunResponse struct {
	AST string `json:"ast"`
	GoCode string `json:"go_code"`
	Output string `json:"output"`
	ExitCode int `json:"exit_code"`
	Error string `json:"error,omitempty"`
}


// main function to handle http requests of users and the web wrapper
func handleRun(w http.ResponseWriter , r *http.Request){
	w.Header().Set("Access-Control-Allow-Origin", "*")
	w.Header().Set("Access-Control-Allow-Methods", "POST, OPTIONS")
	w.Header().Set("Access-Control-Allow-Headers", "Content-Type")


	// check if the method options is requested
	// if it is , return status OK and return
	if r.Method == http.MethodOptions{
		w.WriteHeader(http.StatusOK)
		return
	}

	// if the method requested is not method post
	// output http flag as method not allowed
	if r.Method != http.MethodPost{
		http.Error(w , "Method not allowed" , http.StatusMethodNotAllowed)
		return
	}


	
	body, err := io.ReadAll(r.Body)

	if err != nil {
		http.Error( w , "Failed to read request" , http.StatusBadRequest);
		return
	}


	var req RunRequest

	if err := json.Unmarshal(body , &req); err != nil {
		jsonResponse(w , RunResponse{Error: "Invalid JSON format: " + err.Error()})
		return
	}


	tmpDir := "../output"
	cFilePath := filepath.Join(tmpDir , "c2go_play.c")
	goFilePath := filepath.Join(tmpDir , "main.go")
	
	_ = os.MkdirAll(tmpDir, 0755)
	if err := os.WriteFile(cFilePath , []byte(req.Code) , 0644); err != nil {
		jsonResponse(w , RunResponse{ Error :"Failed to write C file: " + err.Error()})
		return
	}


	c2goBinary := "c2go"
	candidates := []string{"./c2go", "../c2go", "./c2go.exe", "../c2go.exe"}

	for _,candidate := range candidates {
		if _,err := os.Stat(candidate); err == nil {
			c2goBinary = candidate
			break
		}
	}


	cmdTranspile := exec.Command(c2goBinary , "transpiler" , cFilePath , "-o" , goFilePath )
	transpileOut , err := cmdTranspile.CombinedOutput()
	transpileStr := string(transpileOut)

	if err != nil {
		jsonResponse(w , RunResponse{
			AST : extractAST(transpileStr),
			Output : transpileStr,
			ExitCode: 1,
			Error : "Transpilation Failed",
		})

		return
	}


	goCodeBytes, _ := os.ReadFile(goFilePath)
	goCodeStr := string(goCodeBytes)

	cmdRun := exec.Command("go" , "run" , goFilePath)
	runOut , err := cmdRun.CombinedOutput()
	runStr := string(runOut)

	exitCode := 0
	if err != nil {
		if exitErr , ok := err.(*exec.ExitError) ; ok {
			exitCode = exitErr.ExitCode()
		} else {
			exitCode = 1
		}
	}

	jsonResponse( w , RunResponse{
		AST:      extractAST(transpileStr),
		GoCode:   goCodeStr,
		Output:   runStr,
		ExitCode: exitCode,
	})

}


func jsonResponse(w http.ResponseWriter , resp RunResponse){
	w.Header().Set("Content-Type" , "application/json")
	json.NewEncoder(w).Encode(resp)
}


func extractAST(output string) string {
	start := strings.Index(output , "---- Abstract Syntax Tree ---")

	if start == -1 {
		return output
	}


	end := strings.Index(output , "-----------------------------")
	if end == -1 || end <= start {
		return output[start:]
	}

	return strings.TrimSpace(output[start+len("---- Abstract Syntax Tree ---") : end])

}


func main(){
	port := "8080"

	if p := os.Getenv("PORT"); p != "" {
		port = p
	} 

	http.HandleFunc("/api/run" , handleRun)

	staticDir := "../web/build"

	if _, err := os.Stat(staticDir); os.IsNotExist(err) {
		staticDir = "../web/dist"
	}

	if _, err := os.Stat(staticDir) ; err == nil {
		http.Handle("/", http.FileServer(http.Dir(staticDir)))
	} else {
		http.HandleFunc("/" ,  func(w http.ResponseWriter , r *http.Request){
			fmt.Fprintf(w, "🚀 C2Go API Server is running on :%s!\nVisit your Svelte frontend on port 5173 during development.\n", port)
		})
	}

	log.Printf("Web server is running , listening on %s" , port)

	if err := http.ListenAndServe(":"+port, nil); err != nil {
		log.Fatalf("Server failed: %v", err)
	}
}