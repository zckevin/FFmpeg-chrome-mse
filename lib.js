mergeInto(LibraryManager.library, {
  // Returns 0 if the processing is canceled.
  wait_readable: function() {
    return Asyncify.handleSleep(function(wakeUp) {
      waitReadable(wakeUp);
    });
  }
});

