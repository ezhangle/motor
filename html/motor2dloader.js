function runWithFS() {
//  FS.init(null, function(c) { Module.print(String.fromCharCode(c));}, Module.printErr);
  Module['addRunDependency']("zip game.love");
  zip.workerScripts = {
    inflater: ['z-worker.js', 'inflate.js']
  };

  zip.createReader(new zip.HttpReader("game.love"), function(reader) {
    reader.getEntries(function(entries) {
      var i;
      for(i=0; i < entries.length; ++i) {
        if(entries[i].directory) {
          Module['FS_createPath']("/", entries[i].filename, 1, 1);
        } else {
          Module['addRunDependency']("fp " + entries[i].filename);
          entries[i].getData(
            new zip.ArrayBufferWriter(), 
            (function(entry) {
              return function(data) {
                Module['FS_createPreloadedFile']("/" + entry.filename, null, Uint8Array(data), true, true,
                  function() { // ondone
                    //console.log("Preloaded " + entry.filename);
                    Module['removeRunDependency']("fp " + entry.filename);
                  },
                  function() { // onerror
                  },
                  false,
                  false);
              }
            })(entries[i]), 
            
            function() {

            });
        }
      }
      Module['removeRunDependency']('zip game.love');
    });
  }, function(error) {
    
  });
}

if (Module['calledRun']) {
  runWithFS();
} else {
  if (!Module['preRun']) Module['preRun'] = [];
  Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
}  

