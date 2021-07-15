#include "wasm.h"
#include "cJSON.h"
#include "libavutil/avassert.h"
#include "libavformat/movenc.h"

// WasmGlobalConfig *wasm_config = malloc(sizeof(WasmGlobalConfig));
WasmGlobalConfig *wasm_config;

EMSCRIPTEN_KEEPALIVE
int wasm_do_seek(double target)
{
    wasm_config->seek_target = target;
    return 0;
}

EMSCRIPTEN_KEEPALIVE
int wasm_shutdown()
{
    wasm_config->stopped = 1;
    return 0;
}

int wasm_get_avc1_codec_mime(AVStream *stream, char *codec)
{
    // e.g. avc1.4d001f

    AVCodecParameters *par = stream->codecpar;
    int constraint = 0;

    if (par->codec_id != AV_CODEC_ID_H264)
    {
        wasm_msg_callback("{cmd: \"abort\", reason: 1}");
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

double wasm_get_stream_duration(AVFormatContext *ctx, AVStream *stream)
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

void wasm_report_stream_info(AVFormatContext *ctx, AVStream *stream)
{
    char buf[12];
    char *result = NULL;
    cJSON *entry;

    av_assert0(wasm_get_avc1_codec_mime(stream, buf) > 0);

    entry = cJSON_CreateObject();
    if (entry == NULL)
    {
        goto end;
    }

    ADD_KV_TO_CJSON_OBJECT(String, entry, cmd, "meta_info");
    ADD_KV_TO_CJSON_OBJECT(Number, entry, duration, wasm_get_stream_duration(ctx, stream));
    ADD_KV_TO_CJSON_OBJECT(String, entry, codec, buf);

    result = cJSON_PrintUnformatted(entry);

end:
    cJSON_Delete(entry);
    wasm_msg_callback(result ? result : "{}");
}

void wasm_report_moof_mdat_info(double from_seconds, double to_seconds, int moof_size, int mdat_size)
{
    char *result = NULL;
    cJSON *entry;

    entry = cJSON_CreateObject();
    if (entry == NULL)
    {
        goto end;
    }

    ADD_KV_TO_CJSON_OBJECT(String, entry, cmd, "moof_mdat");
    ADD_KV_TO_CJSON_OBJECT(Number, entry, from_seconds, from_seconds);
    ADD_KV_TO_CJSON_OBJECT(Number, entry, to_seconds, to_seconds);
    ADD_KV_TO_CJSON_OBJECT(Number, entry, moof_size, moof_size);
    ADD_KV_TO_CJSON_OBJECT(Number, entry, mdat_size, mdat_size);

    result = cJSON_PrintUnformatted(entry);
    printf("%s\n", result);

end:
    cJSON_Delete(entry);
    wasm_msg_callback(result ? result : "{}");
}

int wasm_almost_equal(double a, double b)
{
    return FFABS(a - b) < 0.001;
}

void wasm_initial_seek(InputStream *ist, AVFormatContext *is)
{
    if (wasm_config->initial_seeked_done == 1 || wasm_almost_equal(wasm_config->seek_target, 0))
    {
        return;
    }
    // if (ist->st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && !wasm_almost_equal(wasm_config->seek_target, -1))
    int ret;
    int64_t target_number = (int64_t)wasm_config->seek_target;
    int64_t target_decimal = (int64_t)((wasm_config->seek_target - (double)target_number) * 10);

    int64_t seek_target_ts = av_rescale_q(target_number * AV_TIME_BASE, AV_TIME_BASE_Q, ist->st->time_base) +
                             av_rescale_q(target_decimal * AV_TIME_BASE, AV_TIME_BASE_Q, ist->st->time_base) / 10;

    wasm_config->initial_seeked_done = 1;
    ret = avformat_seek_file(is, ist->st->index, INT64_MIN, seek_target_ts, seek_target_ts, AVSEEK_FLAG_BACKWARD);
    
    // does not work currently, mov->tracks is nullptr, why?
    /*
    // clear movenc internal buffer
    {
        MOVMuxContext *mov = is->priv_data;
        int i;

        for (i = 0; i < mov->nb_streams; i++)
        {
            MOVTrack *track = &mov->tracks[i];
            track->entry = 0;
            track->entries_flushed = 0;
            track->end_reliable = 0;
            track->mdat_buf = NULL;
        }
    }
    */
    printf("avformat_seek to seconds %f, ts %lld, ret: %d\n", wasm_config->seek_target, seek_target_ts, ret);
}
