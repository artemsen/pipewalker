EMSDK_ENV := /home/art/src.other/emsdk/emsdk_env.sh
CXXFLAGS := -s USE_LIBPNG=1

TARGET_JS  := ./pipewalker.js
TARGET_WASM  := ./pipewalker.wasm

all: $(TARGET_WASM)

$(TARGET_WASM):
	source $(EMSDK_ENV) && cd .. && export CXXFLAGS="-s USE_LIBPNG=1" && emconfigure ./configure
	make -C ..
	cp ../src/pipewalker $(TARGET_JS)
	cp ../src/pipewalker.wasm $(TARGET_WASM)

server:
	python -m http.server 8080

debug:
	firefox http://127.0.0.1:8080

clean:
	rm -f $(TARGET_JS) $(TARGET_WASM)
