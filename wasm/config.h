#ifndef WASM_CONFIG_H
#define WASM_CONFIG_H

struct WasmGlobalConfig;

typedef struct WasmGlobalConfig
{
    // initial seeking target
    double seek_target;

    // if initial seeking is done
    int initial_seeked_done;

    // if video meta info is reported
    int flag_metainfo_uploaded;

    int stopped;
} WasmGlobalConfig;

#endif // WASM_CONFIG_H
