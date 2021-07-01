IS_DEBUG=1
USE_PTHREADS=0

if [ $IS_DEBUG == 0 ]
then
  OPTIMIZATION_FLAGS="-Oz"
  FFMPEG_DEBUG="--disable-debug"
else
  FFMPEG_DEBUG="--enable-debug --disable-stripping"
fi

if [ $USE_PTHREADS == 1 ]
then
  LDFLAGS="-s USE_PTHREADS=1"
  FFMPEG_PTHREADS="--enable-pthreads"
fi

CFLAGS="-s $OPTIMIZATION_FLAGS"

FFMPEG_CONFIG_FLAGS_BASE=(
  $FFMPEG_DEBUG         # disable debug info, required by closure
  $FFMPEG_PTHREADS

  --target-os=none        # use none to prevent any os specific configurations
  --arch=x86_32           # use x86_32 to achieve minimal architectural optimization
  --enable-cross-compile  # enable cross compile
  --disable-x86asm        # disable x86 asm
  --disable-inline-asm    # disable inline asm
  --disable-runtime-cpudetect   # disable runtime cpu detect
  --disable-programs      
  --disable-doc           

  --disable-everything 

  --enable-protocol=file 

  --enable-demuxer=matroska,mov
  --enable-muxer=mov 

  --enable-decoder=h264,mp3,ac3,eac3,vorbis,opus,aac
  --enable-encoder=aac 

  --enable-filter=null,anull,pad,apad,tpad,buffer,buffersink,aformat,format,overlay
  --enable-filter=aresample  # for -channel_layout FC

  --extra-cflags="$CFLAGS"
  --extra-cxxflags="$CFLAGS"
  --extra-ldflags="$LDFLAGS"
  --pkg-config-flags="--static"

  --nm="llvm-nm"
  --ar=emar
  --ranlib=emranlib
  --cc=emcc
  --cxx=em++
  --objcc=emcc
  --dep-cc=emcc
)

# emconfigure ./configure "${FFMPEG_CONFIG_FLAGS_BASE[@]}"
emmake make -j12
