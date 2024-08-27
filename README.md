# C Web Server
A simple (non-complete) HTTP server in C, cross compilable to both Linux and Windows.


Compiling:
```
git clone https://github.com/GreenCobalt/c-webserver
cd c-webserver
make
```

`make` will compile with `cc`, using the default compiler on the system, unless provided with a `linux` or `windows` argument, where it will then compile with `x86_64-linux-gnu-gcc` or `x86_64-w64-mingw32-gcc` respectively.

Running:
```
./server
```
Will open a socket on all interfaces, port 9000 (by default). Serves files in webroot/.
