#include "libavutil/avassert.h"
#include "libavformat/movenc.h"

#include "wasm/wasm.h"
#include "wasm/cJSON.h"

WasmGlobalConfig *wasm_config;

void wasm_report_moof_mdat_info(double from_seconds, double to_seconds, int moof_size, int mdat_size)
{
    char *result = NULL;
    cJSON *entry;

    entry = cJSON_CreateObject();
    if (entry == NULL)
    {
        goto end;
    }

    ADD_KV_TO_CJSON_OBJECT(Number, entry, from_seconds, from_seconds);
    ADD_KV_TO_CJSON_OBJECT(Number, entry, to_seconds, to_seconds);
    ADD_KV_TO_CJSON_OBJECT(Number, entry, moof_size, moof_size);
    ADD_KV_TO_CJSON_OBJECT(Number, entry, mdat_size, mdat_size);

    result = cJSON_PrintUnformatted(entry);
    printf("%s\n", result);

end:
    cJSON_Delete(entry);
    wasm_js_msg_callback("moof_mdat", result ? result : "{}");
}

int get_avc1_codec_mime(AVStream *stream, char *codec)
{
    // e.g. avc1.4d001f

    AVCodecParameters *par = stream->codecpar;
    int constraint = 0;

    if (par->codec_id != AV_CODEC_ID_H264)
    {
        wasm_js_msg_callback("error", "{reason: \"video codec not h264\"}");
        exit_program(1);
    }

    if (par->profile & FF_PROFILE_H264_CONSTRAINED)
    {
        constraint = constraint | (1 << 6);
    }
    if (par->profile & FF_PROFILE_H264_INTRA)
    {
        constraint = constraint | (1 << 4);
    }

    return sprintf(codec, "avc1.%02x%02x%02x", (par->profile & 0xff), constraint, (par->level & 0xff));
}

double get_stream_duration(AVFormatContext *ctx, AVStream *stream)
{
    double duration;
    // stream->duration may return garbage if no transcode is needed
    duration = stream->duration * av_q2d(stream->time_base);

    if (duration > 3600 * 24 || duration <= 0)
    {
        duration = ctx->duration * av_q2d(stream->time_base) / 1000;
        if (duration > 3600 * 24)
        {
            duration = duration / 1000;
        }
    }

    if (duration > 3600 * 24 || duration <= 0)
    {
        return -1;
    }
    return duration;
}

void wasm_upload_metainfo(AVFormatContext *ctx, AVStream *stream)
{
    char buf[12];
    char *result = NULL;
    cJSON *entry;

    av_assert0(get_avc1_codec_mime(stream, buf) > 0);

    entry = cJSON_CreateObject();
    if (entry == NULL)
    {
        goto end;
    }

    ADD_KV_TO_CJSON_OBJECT(Number, entry, duration, get_stream_duration(ctx, stream));
    ADD_KV_TO_CJSON_OBJECT(String, entry, codec, buf);

    result = cJSON_PrintUnformatted(entry);

end:
    cJSON_Delete(entry);
    wasm_js_msg_callback("meta_info", result ? result : "{}");
}