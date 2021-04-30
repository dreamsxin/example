
## 安装

```shell
go get -u github.com/ofabry/go-callvis
cd $GOPATH/src/github.com/ofabry/go-callvis && make

go-callvis [flags] package
```

## 使用

```out
Usage of go-callvis:
  -debug
    	Enable verbose log.
  -file string
    	output filename - omit to use server mode
  -cacheDir string
    	Enable caching to avoid unnecessary re-rendering.
  -focus string
    	Focus specific package using name or import path. (default "main")
  -format string
    	output file format [svg | png | jpg | ...] (default "svg")
  -graphviz
    	Use Graphviz's dot program to render images.
  -group string
    	Grouping functions by packages and/or types [pkg, type] (separated by comma) (default "pkg")
  -http string
    	HTTP service address. (default ":7878")
  -ignore string
    	Ignore package paths containing given prefixes (separated by comma)
  -include string
    	Include package paths with given prefixes (separated by comma)
  -limit string
    	Limit package paths to given prefixes (separated by comma)
  -minlen uint
    	Minimum edge length (for wider output). (default 2)
  -nodesep float
    	Minimum space between two adjacent nodes in the same rank (for taller output). (default 0.35)
  -nointer
    	Omit calls to unexported functions.
  -nostd
    	Omit calls to/from packages in standard library.
  -skipbrowser
    	Skip opening browser.
  -tags build tags
    	a list of build tags to consider satisfied during the build. For more information about build tags, see the description of build constraints in the documentation for the go/build package
  -tests
    	Include test code.
  -version
    	Show version and exit.
```

* Generate graph and launch webserver
```shell
go-callvis 
  -focus upgrade -group pkg,type 
  -limit github.com/syncthing/syncthing 
  -ignore github.com/syncthing/syncthing/lib/logger
  github.com/syncthing/syncthing/cmd/syncthing
```

```shell
go-callvis -format=png -file=syncthing_focus -focus upgrade -limit github.com/syncthing/syncthing github.com/syncthing/syncthing/cmd/syncthing
```

* Generate graph focused on module 'upgrade', output to PNG file
```shell
go-callvis -format=png -file=syncthing_group -focus upgrade -group pkg -limit github.com/syncthing/syncthing github.com/syncthing/syncthing/cmd/syncthing
```

* Generate graph focused on module 'upgrade' and ignoring 'logger', output to webserver
```shell
go-callvis -focus upgrade -group pkg -ignore github.com/syncthing/syncthing/lib/logger -limit github.com/syncthing/syncthing github.com/syncthing/syncthing/cmd/syncthing
```

```shell
go-callvis -format=png -file=docker -limit github.com/docker/docker -ignore github.com/docker/docker/vendor github.com/docker/docker/cmd/docker | dot -Tpng -o docker.png
go-callvis -format=svg -file=travis -minlen 3 -nostd -group type,pkg -focus worker -limit github.com/travis-ci/worker -ignore github.com/travis-ci/worker/vendor github.com/travis-ci/worker/cmd/travis-worker && exo-open travis.svg
```

* go-kit

```shell
cd /go/src/github.com/go-kit/kit/examples/apigateway
go-callvis -group pkg,type -file kit.svg main.go
```
