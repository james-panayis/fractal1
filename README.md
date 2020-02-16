## Introduction

Simple Mandelbrot visualizer  

Calculates more detail over time
Scroll mousewheel or use *f* and *g* to zoom centered on mouse pointer  
Use *wasd* to move  
Click when zooming to stop zooming  
Click when still to reset  
  
Written as a WebGL shader  
  
For demo please see: https://jamespanayis.com/fractal1

## Build instructions

The C++ is compiled to WebAssembly and requires a working **emscripten** install  
  
run:

	make


## Install instructions

Run a local webserver and surf to it eg:

	python -m SimpleHTTPServer 8080

Browse to http://localhost:8080/build/fields1.html

OR:

Copy files to webserver root:

	PREFIX=/path/to/web/root make install
	
Browse to your webserver.

