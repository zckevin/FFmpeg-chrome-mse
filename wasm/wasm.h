#ifndef WASM_H
#define WASM_H

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "fftools/ffmpeg.h"

#include <emscripten.h>
#include "wasm/config.h"

void wasm_report_moof_mdat_info(double from_seconds, double to_seconds, int moof_size, int mdat_size);
void wasm_upload_metainfo(AVFormatContext *ctx, AVStream *stream);

int wasm_set_seek_target(double target);
int wasm_stop();
void wasm_initial_seek(InputStream *ist, AVFormatContext *is);

/****************************************************
 * javascript lib functions
****************************************************/
extern int wasm_js_wait_read_result();
extern int wasm_js_do_snapshot();
extern int wasm_js_has_seeked();
extern int wasm_js_msg_callback(const char* name, const char* msg);
extern int wasm_js_pause_decode(double pkt_pts_seconds, int is_eof);

#define ADD_KV_TO_CJSON_OBJECT(type, obj, key, value)          \
    do                                                         \
    {                                                          \
        cJSON *key##_json_object_ = cJSON_Create##type(value); \
        if (key##_json_object_ == NULL)                        \
        {                                                      \
            goto end;                                          \
        }                                                      \
        cJSON_AddItemToObject(obj, #key, key##_json_object_);  \
    } while (0);

#endif // WASM_H
