package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"strings"

	"github.com/comail/colog"
	"github.com/julienschmidt/httprouter"

	v1 "k8s.io/api/core/v1"
	"k8s.io/apimachinery/pkg/types"
	schedulerapi "k8s.io/kubernetes/pkg/scheduler/api"
)

const (
	versionPath      = "/version"
	apiPrefix        = "/scheduler"
	bindPath         = apiPrefix + "/bind"
	preemptionPath   = apiPrefix + "/preemption"
	predicatesPrefix = apiPrefix + "/predicates"
	prioritiesPrefix = apiPrefix + "/priorities"

	defaultHTTPPort = "80"
	keyHTTPPort     = "HTTP_PORT"
)

var (
	// injected via ldflags at build time
	version string

	// switch to initialize operation only once
	initialized = false

	// the http router
	router *httprouter.Router

	// NoBind :
	NoBind = Bind{
		Func: func(podName string, podNamespace string, podUID types.UID, node string) error {
			return fmt.Errorf("Extender doesn't support Bind: make 'BindVerb' be empty in your ExtenderConfig")
		},
	}

	// EchoPreemption :
	EchoPreemption = Preemption{
		Func: func(
			_ v1.Pod,
			_ map[string]*schedulerapi.Victims,
			nodeNameToMetaVictims map[string]*schedulerapi.MetaVictims,
		) map[string]*schedulerapi.MetaVictims {
			return nodeNameToMetaVictims
		},
	}
)

func init() {
	InitMain()
}

// InitMain : initialize main - should be done before any other init()
func InitMain() {
	if !initialized {
		// init logger
		initLogger()

		// init router
		router = httprouter.New()
		AddVersion(router)

		initialized = true
	}
}

// Initialize logger
func initLogger() {
	colog.SetDefaultLevel(colog.LInfo)
	colog.SetMinLevel(colog.LInfo)
	colog.SetFormatter(&colog.StdFormatter{
		Colors: true,
		Flag:   log.Ldate | log.Ltime | log.Lshortfile,
	})
	colog.Register()
	level := StringToLevel(os.Getenv("LOG_LEVEL"))
	log.Print("Log level was set to ", strings.ToUpper(level.String()))
	colog.SetMinLevel(level)
}

// StringToLevel : determine log level
func StringToLevel(levelStr string) colog.Level {
	switch level := strings.ToUpper(levelStr); level {
	case "TRACE":
		return colog.LTrace
	case "DEBUG":
		return colog.LDebug
	case "INFO":
		return colog.LInfo
	case "WARNING":
		return colog.LWarning
	case "ERROR":
		return colog.LError
	case "ALERT":
		return colog.LAlert
	default:
		log.Printf("warning: LOG_LEVEL=\"%s\" is empty or invalid, fallling back to \"INFO\".\n", level)
		return colog.LInfo
	}
}

// AddRouterPredicate : add a predicate to the router
func AddRouterPredicate(p Predicate) {
	AddPredicate(router, p)
}

// AddRouterPriority : add a priority function to the router
func AddRouterPriority(p Prioritize) {
	AddPrioritize(router, p)
}

func main() {
	AddBind(router, NoBind)

	// start http server
	hostPort := defaultHTTPPort
	if hp := os.Getenv(keyHTTPPort); len(hp) > 0 {
		hostPort = hp
	}

	log.Printf("info: server starting on port %s \n", hostPort)
	if err := http.ListenAndServe(":"+hostPort, router); err != nil {
		log.Fatal(err)
	}
}
