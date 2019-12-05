package main

import (
	"github.com/comail/colog"
	"github.com/julienschmidt/httprouter"
	"log"
	"net/http"
	"os"
	"strings"

	k8s_scheduler_extender "github.com/wangchen615/k8s-scheduler-extender-example/k8s_scheduler_extender"
)

func main() {
	colog.SetDefaultLevel(colog.LDebug)
	colog.SetMinLevel(colog.LDebug)
	colog.SetFormatter(&colog.StdFormatter{
		Colors: true,
		Flag:   log.Ldate | log.Ltime | log.Lshortfile,
	})
	colog.Register()
	level := k8s_scheduler_extender.StringToLevel(os.Getenv("LOG_LEVEL"))
	log.Print("Log level was set to ", strings.ToUpper(level.String()))
	colog.SetMinLevel(level)

	router := httprouter.New()
	k8s_scheduler_extender.AddVersion(router)

	// priorities := []Prioritize{SafeBalancePriority, SafeOverloadPriority, CongestionPriority}
	priorities := []k8s_scheduler_extender.Prioritize{RLPriority}

	for _, p := range priorities {
		k8s_scheduler_extender.AddPrioritize(router, p)
	}


	log.Print("info: server starting on the port :80")
	if err := http.ListenAndServe(":80", router); err != nil {
		log.Fatal(err)
	}
}


