IS_DEBUG=1
USE_PTHREADS=0

if [ $IS_DEBUG == 0 ]
then
  OPTIMIZATION_FLAGS="-Oz"
else
  # gen DWARF symbols
  OPTIMIZATION_FLAGS="-g"
fi

if [ $USE_PTHREADS == 1 ]
then
  WASM_PTHREADS="-pthread -s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=1 -s PTHREADS_DEBUG=1"
fi

# use commonjs instead of esm
# -lnodefs.js
# WASM_ENV_NODE="-s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=1 -s PTHREADS_DEBUG=1 -s MODULARIZE -s EXPORT_NAME=ffmpeg -s ENVIRONMENT='node' -o /home/zc/PROJECTS/wasm-mse-player/wasm/ffmpeg.node.cjs"
WASM_ENV_NODE="-s MODULARIZE -s EXPORT_NAME=ffmpeg -s ENVIRONMENT='node' -o /home/zc/PROJECTS/wasm-mse-player/wasm/ffmpeg.node.cjs"

WASM_ENV_WEB="-s MODULARIZE -s EXPORT_ES6=1 -s ENVIRONMENT='web,worker' -o /home/zc/PROJECTS/wasm-mse-player/wasm/ffmpeg.web.js"

ARGS=(
  $OPTIMIZATION_FLAGS
  $WASM_PTHREADS
  $WASM_ENV

  -Qunused-arguments
  -I.
  -L./libavcodec -L./libavformat -L./libswresample -L./libavutil -L./libavfilter -L./libavdevice -L./libswscale
	-lavformat -lavutil -lavcodec -lswresample -lavfilter -lavdevice -lswscale 
	fftools/ffmpeg_opt.c fftools/ffmpeg_filter.c fftools/ffmpeg_hw.c fftools/cmdutils.c fftools/ffmpeg.c fftools/cJSON.c fftools/wasm.c
	-o /home/zc/PROJECTS/wasm-mse-player/wasm/ffmpeg.js

  # -s PTHREAD_POOL_SIZE=4 -s PTHREAD_POOL_SIZE_STRICT=2 

  # -s EXPORTED_FUNCTIONS="[_main, _emscripten_proxy_main, _add_js_callback, _wasm_do_seek, _malloc]"  # export main and proxy_main funcs
  -s EXPORTED_FUNCTIONS="[_main, _wasm_do_seek, _transcode_second_part, _wasm_shutdown, _malloc]"  # export main and proxy_main funcs
  -s INVOKE_RUN=0                               # not to run the main() in the beginning
  -s 'EXPORTED_RUNTIME_METHODS=["FS", "ccall", "cwrap", "writeAsciiToMemory", "setValue", "lengthBytesUTF8", "stringToUTF8", "UTF8ToString", "addFunction", "allocate", "intArrayFromString", "ALLOC_NORMAL"]'

  # --preload-file assets
  -s INITIAL_MEMORY=134217728      # 128 MB
  -s ALLOW_MEMORY_GROWTH=0

  -s ALLOW_TABLE_GROWTH=1     # for Module.addFunction()

  -s EXIT_RUNTIME=1

  -s 'ASYNCIFY_IMPORTS=[wasm_wait_readable, wasm_pause_decode, wasm_sleep]'
  -s ASYNCIFY
  -s ASYNCIFY_STACK_SIZE=300000
  --js-library ./lib.js
)

sh ./build-ffmpeg-lib-wasm.sh
# emcc -v "${ARGS[@]}" $WASM_ENV_NODE
emcc -v "${ARGS[@]}" $WASM_ENV_WEB

