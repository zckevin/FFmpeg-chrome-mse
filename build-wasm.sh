IS_DEBUG=1
USE_PTHREADS=0

IS_NODE_JS_ENV=0

if [ $IS_DEBUG == 0 ]
then
  OPTIMIZATION_FLAGS="-Oz"
fi

if [ $USE_PTHREADS == 1 ]
then
  WASM_PTHREADS="-s USE_PTHREADS=1 -s PROXY_TO_PTHREAD=1 -s PTHREADS_DEBUG=1"
fi

if [ $IS_NODE_JS_ENV == 1 ]
then
  WASM_ENV="-s ENVIRONMENT='node' -lnodefs.js"
else
  WASM_ENV="-s MODULARIZE -s EXPORT_ES6=1 -s ENVIRONMENT='web,worker'"

fi

ARGS=(
  $OPTIMIZATION_FLAGS
  $WASM_PTHREADS
  $WASM_ENV

  -Qunused-arguments
  -I.
  -L./libavcodec -L./libavformat -L./libswresample -L./libavutil -L./libavfilter -L./libavdevice -L./libswscale
	-lavformat -lavutil -lavcodec -lswresample -lavfilter -lavdevice -lswscale 
	fftools/ffmpeg_opt.c fftools/ffmpeg_filter.c fftools/ffmpeg_hw.c fftools/cmdutils.c fftools/ffmpeg.c fftools/cJSON.c
	-o /home/zc/PROJECTS/wasm-mse-player/dist/ffmpeg.js

	# -pthread
  # -s USE_PTHREADS=1                             # enable pthreads support
  # -s PROXY_TO_PTHREAD=1                         # detach main() from browser/UI main thread
  # -s PTHREAD_POOL_SIZE=4 -s PTHREAD_POOL_SIZE_STRICT=2 

  -s EXPORTED_FUNCTIONS="[_main, _add_js_callback, _malloc]"  # export main and proxy_main funcs
  -s INVOKE_RUN=0                               # not to run the main() in the beginning
  -s 'EXPORTED_RUNTIME_METHODS=["FS", "ccall", "cwrap", "writeAsciiToMemory", "setValue", "lengthBytesUTF8", "stringToUTF8", "UTF8ToString", "addFunction", "allocate", "intArrayFromString", "ALLOC_NORMAL"]'

  # --preload-file assets
  -s INITIAL_MEMORY=134217728      # 33554432 bytes = 128 MB
  # -s ALLOW_MEMORY_GROWTH=1
  -gsource-map --source-map-base /

  -s ALLOW_TABLE_GROWTH=1     # for Module.addFunction()

  -s EXIT_RUNTIME=1

  -s 'ASYNCIFY_IMPORTS=["wait_readable"]'
  -s ASYNCIFY
  -s ASYNCIFY_STACK_SIZE=300000
  --js-library ./lib.js
)

emcc -v "${ARGS[@]}"

