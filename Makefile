# destination name
NAME=fractal1

# installation prefix
#PREFIX=/data/wa

# the webpath for becomes the installation prefix followed by the program name
WEBPATH=${PREFIX}/${NAME}

all: native web
native: build-native/$(NAME)
web: build-web/$(NAME).html

COMMON_DEPS_SRC= src/main.cpp src/image.hpp
COMMON_DEPS_LIBS= libs/ui_event_interface.hpp

NATIVE_CC=g++
NATIVE_CFLAGS=-std=c++17 -O2 -I./
NATIVE_OPTIONS=-L/usr/local/lib -lglfw3 -lrt -lm -ldl -lX11 -lpthread -lxcb -lXau -lXdmcp -lGL

NATIVE_DEPS= $(COMMON_DEPS_SRC) $(COMMON_DEPS_LIBS) libs/ui_event_impl_glfw.hpp

build-native/$(NAME): $(NATIVE_DEPS) 
	mkdir -p build-native
	$(NATIVE_CC) $(NATIVE_CFLAGS) src/main.cpp $(NATIVE_OPTIONS) -o build-native/$(NAME)
	strip build-native/$(NAME)

install-native:
	cp build-native/$(NAME) $(PREFIX)/


WEB_CC=emcc
WEB_CFLAGS=-std=c++17 -Os -I./
WEB_OPTIONS=-s WASM=1 -s USE_WEBGL2=0 -s FILESYSTEM=0 -s ENVIRONMENT='web' --closure 1
WEB_DEPS= $(COMMON_DEPS_SRC) $(COMMON_DEPS_LIBS) src/shell_fullscreen.html libs/ui_event_impl_web.hpp

build-web/$(NAME).html: $(WEB_DEPS)
	mkdir -p build-web
	$(WEB_CC) $(WEB_CFLAGS) src/main.cpp $(WEB_OPTIONS) -o build-web/$(NAME).html --shell-file src/shell_fullscreen.html

# copy the web files to the web server path and generate gzip and brotli compressed versions
install-web:
	mkdir -p ${WEBPATH}
	cp build-web/${NAME}.html ${WEBPATH}/index.html 
	cp build-web/$(NAME).js build-web/$(NAME).wasm $(WEBPATH)/
	cd $(WEBPATH); brotli -f index.html -o index.html.br; brotli -f $(NAME).js -o $(NAME).js.br; brotli -f $(NAME).wasm -o $(NAME).wasm.br
	cd $(WEBPATH); gzip -f -k index.html; gzip -f -k $(NAME).js; gzip -f -k $(NAME).wasm

clean:
	rm -rf build-native build-web
