mergeInto(LibraryManager.library, {
  // Returns 0 if the processing is canceled.
  wasm_wait_readable: function() {
    return Asyncify.handleSleep(function(wakeUp) {
      waitReadable(wakeUp);
    });
  },

  wasm_pause_decode: function(...args) {
    return Asyncify.handleSleep(function(wakeUp) {
      pauseDecodeIfNeeded(wakeUp, ...args);
    });
  }
});

