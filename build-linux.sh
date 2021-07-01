IS_DEBUG=1
USE_PTHREADS=0

if [ $IS_DEBUG == 0 ]
then
  OPTIMIZATION_FLAGS="-Oz"
  FFMPEG_DEBUG="--disable-debug"
else
  FFMPEG_DEBUG="--enable-debug --disable-stripping"
fi

CFLAGS="-s $OPTIMIZATION_FLAGS"

if [ $USE_PTHREADS == 1 ]
then
  FFMPEG_PTHREADS="--enable-pthreads"
fi

FFMPEG_CONFIG_FLAGS_BASE=(
  $FFMPEG_DEBUG         # disable debug info, required by closure
  $FFMPEG_PTHREADS

  --disable-x86asm      # no nasm installed

  --disable-everything 
  --disable-doc           # disable doc

  --enable-protocol=file 

  --enable-demuxer=matroska,mov
  --enable-muxer=mov 

  --enable-decoder=h264,mp3,ac3,eac3
  --enable-encoder=aac 

  --enable-filter=null,anull,pad,apad,tpad,buffer,buffersink,aformat,format,overlay
  --enable-filter=aresample  # for -channel_layout FC

  --extra-cflags="$CFLAGS"
  --extra-cxxflags="$CFLAGS"
  --extra-ldflags="$LDFLAGS"
)

# ./configure "${FFMPEG_CONFIG_FLAGS_BASE[@]}"
make -j12
make install
