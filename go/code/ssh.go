package main
 
import (
	"github.com/gliderlabs/ssh"
	gossh "golang.org/x/crypto/ssh"
	"io/ioutil"
	"log"
)
 
func homeHandler(s ssh.Session) {
 
}
 
func passwordHandler(ctx ssh.Context, password string) bool {
	log.Printf("username:%s, password:%s\n", ctx.User(), password)
	return true
}
 
func main() {
	keyPath := "id_rsa"
	keyData, err := ioutil.ReadFile(keyPath)
	if err != nil {
		log.Fatal(err)
		return
	}
	key, err := gossh.ParsePrivateKey(keyData)
	if err != nil {
		log.Fatal(err)
		return
	}
 
	s := &ssh.Server{
		Addr:    "127.0.0.1:22022",
		Handler: homeHandler, //
		//PublicKeyHandler:
		PasswordHandler: passwordHandler,
	}
	s.AddHostKey(key)
	log.Fatal(s.ListenAndServe())
}
