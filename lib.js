mergeInto(LibraryManager.library, {
  // Returns 0 if the processing is canceled.
  wasm_wait_readable: function () {
    return Asyncify.handleSleep(function (wakeUp) {
      waitReadable(wakeUp);
    });
  },

  wasm_pause_decode: function (...args) {
    console.log("enter wasm_pause_decode()", ...args)
    return Asyncify.handleSleep(function (wakeUp) {
      pauseDecodeIfNeeded((seekingBack) => {
        if (seekingBack) {
          console.log("abort wasm_pause_decode()", ...args)
          wakeUp(1);
        } else {
          console.log("exit wasm_pause_decode()", ...args)
          wakeUp(0);
        }
      }, ...args);
    });
  },

  wasm_sleep: function () {
    return Asyncify.handleSleep(function (wakeUp) {
      // worker global, not window global
      globalThis.__wakeup = wakeUp;
    });
  },

  wasm_msg_callback: function (...args) {
    __ffmpeg_msg_callback(...args);
  },
});
