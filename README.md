## Introduction

Simple Mandelbrot visualizer  
  
Written as a WebGL shader  
  
Calculates more detail over time
Scroll mousewheel or use *f* and *g* to zoom centered on mouse pointer  
Use *wasd* to move  
Click when zooming to stop zooming  
Click when still to reset  
  
For demo please see: https://jamespanayis.com/fractal1

## Build instructions - Web

The C++ is compiled to WebAssembly and requires a working **emscripten** install  
  
run:

	make web


## Install instructions - Web

Run a local webserver and surf to it, eg:

	python -m http.server 8080

Browse to http://localhost:8080/build-web/fractal1.html

OR:

Copy files to webserver root:

	PREFIX=/path/to/web/root make install-web
	
Browse to your webserver.


## Build instructions - Native

The C++ is compiled to a native binary and requires a working **glfw3** install  
  
run:

	make native

## Install instructions - Native

To execute, run:

	build-native/fractal1

OR:

Copy binary to executable path, eg /usr/local/bin:

	sudo PREFIX=/usr/local/bin make install-native

Execute fractal1