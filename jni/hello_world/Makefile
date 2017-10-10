all: run

helloWorld.class: helloWorld.java
	/usr/java/latest/bin/javac helloWorld.java

hello_world: hello_world.c
	gcc -o hello_world \
	-L /usr/java/latest/jre/lib/amd64/server/ \
	-ljvm \
	-I /usr/java/latest/include/ \
	-I /usr/java/latest/include/linux/ \
	hello_world.c

run: helloWorld.class hello_world
	export LD_LIBRARY_PATH="/usr/java/latest/jre/lib/amd64/server/"
	./hello_world

clean:
	rm -f helloWorld.class hello_world
