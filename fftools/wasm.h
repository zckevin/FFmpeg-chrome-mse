#ifndef WASM_H
#define WASM_H

#include <emscripten/emscripten.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "fftools/ffmpeg.h"
#include "wasm-config.h"

/****************************************************
 * javascript lib functions
****************************************************/
extern int wasm_sleep(void);
extern int wasm_pause_decode(double pkt_pts_seconds, int at_eof);
extern int wasm_msg_callback(const char* msg);

/****************************************************
 * global status variable
****************************************************/

// EMSCRIPTEN_KEEPALIVE
int wasm_do_seek(double target);

int wasm_get_avc1_codec_mime(AVStream *stream, char *codec);
double wasm_get_stream_duration(AVFormatContext *ctx, AVStream *stream);

void wasm_report_stream_info(AVFormatContext *ctx, AVStream *stream);
void wasm_report_moof_mdat_info(double from_seconds, double to_seconds, int moof_size, int mdat_size);

void wasm_initial_seek(InputStream *ist, AVFormatContext *is);

#endif // WASM_H