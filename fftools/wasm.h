#include <emscripten/emscripten.h>
#include "libavcodec/avcodec.h"
#include "cJSON.h"

typedef void(*WasmJSCallback)(const char*);

WasmJSCallback wasm_g_js_callback = NULL;
int wasm_flag_meta_info_reported = 0;

int wasm_get_avc1_codec_mime(AVStream *stream, char *codec);
double wasm_get_stream_duration(AVFormatContext *ctx, AVStream *stream);
int wasm_main(WasmJSCallback cb, int argc, char **argv);
void wasm_report_stream_info(AVFormatContext *ctx, AVStream *stream);

int wasm_get_avc1_codec_mime(AVStream *stream, char *codec)
{
  // e.g. avc1.4d001f

  AVCodecParameters *par = stream->codecpar;
  int constraint = 0;

  if (par->codec_id != AV_CODEC_ID_H264) {
    wasm_g_js_callback("{cmd: \"abort\", reason: 1}");
    exit_program(1);
  }

  if (par->profile & FF_PROFILE_H264_CONSTRAINED) {
    constraint = constraint | (1<<6);
  }
  if (par->profile & FF_PROFILE_H264_INTRA) {
    constraint = constraint | (1<<4);
  }

  return sprintf(codec, "avc1.%02x%02x%02x", (par->profile & 0xff), constraint, (par->level & 0xff));
}

double wasm_get_stream_duration(AVFormatContext *ctx, AVStream *stream)
{
  double duration;
  // stream->duration may return garbage if no transcode is needed
  duration = stream->duration * av_q2d(stream->time_base);

  if (duration > 3600 * 24 || duration <= 0) {
    duration = ctx->duration * av_q2d(stream->time_base) / 1000;
    if (duration > 3600 * 24)  {
      duration = duration / 1000;
    }
  }
  
  if (duration > 3600 * 24 || duration <= 0) {
    return -1;
  }
  return duration;
}

#define ADD_KV_TO_CJSON_OBJECT(type, obj, key, value) \
  key = cJSON_Create##type(value); \
  if (key == NULL) { \
    goto end; \
  } \
  cJSON_AddItemToObject(obj, #key, key);

void wasm_report_stream_info(AVFormatContext *ctx, AVStream *stream)
{
  char buf[12];
  char *result = NULL;
  cJSON *entry, *cmd, *duration, *codec;

  av_assert0(wasm_get_avc1_codec_mime(stream, buf) > 0);

  entry = cJSON_CreateObject();
  if (entry == NULL) {
    goto end;
  }

  ADD_KV_TO_CJSON_OBJECT(String, entry, cmd, "meta_info");
  ADD_KV_TO_CJSON_OBJECT(Number, entry, duration, wasm_get_stream_duration(ctx, stream));
  ADD_KV_TO_CJSON_OBJECT(String, entry, codec, buf);

  result = cJSON_Print(entry);

end:
  cJSON_Delete(entry);
  wasm_g_js_callback(result ? result : "{}");
}
