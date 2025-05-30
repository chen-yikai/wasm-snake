#!/bin/bash

emcc snake.c -o snake.js \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_main", "_handle_key"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s NO_EXIT_RUNTIME=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s ASSERTIONS=1 \
    -s SAFE_HEAP=1 \
    -g

echo "Done!"
