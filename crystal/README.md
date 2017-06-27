
```shell
sudo apt-get install llvm build-essential git

git clone https://github.com/ivmai/bdwgc.git
cd bdwgc
git clone https://github.com/ivmai/libatomic_ops.git
autoreconf -vif
automake --add-missing
./configure
make
make check
sudo make install
```

```shell
sudo apt-get install \
  libbsd-dev \
  libedit-dev \
  libevent-core-2.0-5 \
  libevent-dev \
  libevent-extra-2.0-5 \
  libevent-openssl-2.0-5 \
  libevent-pthreads-2.0-5 \
  libgmp-dev \
  libgmpxx4ldbl \
  libssl-dev \
  libxml2-dev \
  libyaml-dev \
  libreadline-dev \
  automake \
  libtool \
  libpcre3-dev \
```

```shell
git clone https://github.com/crystal-lang/crystal.git
cd crystal
make
```

- Run `make` to build your own version of the compiler
- Run `make spec` to ensure all specs pass, and you've installed everything correctly.
- Use `bin/crystal` to run your crystal files

```crystal
# A very basic HTTP server
require "http/server"

server = HTTP::Server.new(8080) do |context|
  context.response.content_type = "text/plain"
  context.response.print "Hello world, got #{context.request.path}!"
end

puts "Listening on http://127.0.0.1:8080"
server.listen
```
