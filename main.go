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
)

var (
	version string // injected via ldflags at build time

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

func main() {
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

	router := httprouter.New()
	AddVersion(router)

	// define set of predicates
	predicates := []Predicate{SafeOverloadPredicate}
	for _, p := range predicates {
		AddPredicate(router, p)
	}

	// define set of priority functions
	priorities := []Prioritize{SafeBalancePriority, SafeOverloadPriority, CongestionPriority}
	for _, p := range priorities {
		AddPrioritize(router, p)
	}

	AddBind(router, NoBind)

	log.Print("info: server starting on the port :80")
	if err := http.ListenAndServe(":80", router); err != nil {
		log.Fatal(err)
	}
}
