package main

import (
	"fmt"
	"log"
	"net/http"
)

type String string

func (s String) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	switch r.URL.Path {
	case "/":
		fmt.Fprint(w, s)
	case "/hello":
		fmt.Fprint(w, s)
	default:
		fmt.Fprint(w, "Hello")
	}
}

func main() {
	http.Handle("/", String("I'm a root."))
	http.Handle("/hello", String("I'm a hello."))

	err := http.ListenAndServe("localhost:4000", nil)

	if err != nil {
		log.Fatal(err)
	}
}