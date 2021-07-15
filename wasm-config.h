#ifndef WASM_CONFIG_H
#define WASM_CONFIG_H

struct WasmGlobalConfig;

typedef void (*wasm_report_moof_mdat_info_func) (double, double, int, int);

typedef struct WasmGlobalConfig
{
    // initial seeking target
    double seek_target;

    // if initial seeking is done
    int initial_seeked_done;

    // if video meta info is reported
    int flag_meta_info_reported;

    int stopped;

    wasm_report_moof_mdat_info_func wasm_report_moof_mdat_info;
} WasmGlobalConfig;

#endif // WASM_CONFIG_H