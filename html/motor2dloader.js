var statusElement = document.getElementById('status');
var progressElement = document.getElementById('progress');
var spinnerElement = document.getElementById('spinner');

var Module = {
  preRun: [/*function() { FS.init(function(){}, Module.print, Module.printErr); }*/],
  postRun: [],
  print: (function() {
    var element = document.getElementById('output');
    if (element) element.value = ''; // clear browser cache
    return function(text) {
      text = Array.prototype.slice.call(arguments).join(' ');
      // These replacements are necessary if you render to raw HTML
      //text = text.replace(/&/g, "&amp;");
      //text = text.replace(/</g, "&lt;");
      //text = text.replace(/>/g, "&gt;");
      //text = text.replace('\n', '<br>', 'g');
      console.log(text);
      if (element) {
        element.value += text + "\n";
        element.scrollTop = element.scrollHeight; // focus on bottom
      }
    };
  })(),
  printErr: function(text) {
    text = Array.prototype.slice.call(arguments).join(' ');
    if (0) { // XXX disabled for safety typeof dump == 'function') {
      dump(text + '\n'); // fast, straight to the real console
    } else {
      console.error(text);
    }
  },
  canvas: (function() {
    var canvas = document.getElementById('canvas');

    // As a default initial behavior, pop up an alert when webgl context is lost. To make your
    // application robust, you may want to override this behavior before shipping!
    // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
    canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

    return canvas;
  })(),
  setStatus: function(text) {
    if (!Module.setStatus.last) Module.setStatus.last = { time: Date.now(), text: '' };
    if (text === Module.setStatus.text) return;
    var m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
    var now = Date.now();
    if (m && now - Date.now() < 30) return; // if this is a progress update, skip it if too soon
    if (m) {
      text = m[1];
      progressElement.value = parseInt(m[2])*100;
      progressElement.max = parseInt(m[4])*100;
      progressElement.hidden = false;
      spinnerElement.hidden = false;
    } else {
      progressElement.value = null;
      progressElement.max = null;
      progressElement.hidden = true;
      if (!text) spinnerElement.style.display = 'none';
    }
    statusElement.innerHTML = text;
  },
  totalDependencies: 0,
  monitorRunDependencies: function(left) {
    this.totalDependencies = Math.max(this.totalDependencies, left);
    Module.setStatus(left ? 'Preparing... (' + (this.totalDependencies-left) + '/' + this.totalDependencies + ')' : 'All downloads complete.');
  }
};
Module.setStatus('Downloading...');
window.onerror = function(event) {
  // TODO: do not warn on ok events like simulating an infinite loop or exitStatus
  Module.setStatus('Exception thrown, see JavaScript console');
  spinnerElement.style.display = 'none';
  Module.setStatus = function(text) {
    if (text) Module.printErr('[post-exception status] ' + text);
  };
};
Module.TOTAL_MEMORY=67108864;

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
        console.log("Entry: " + entries[i].filename);
        if(entries[i].directory) {
          Module['FS_createPath']("/", entries[i].filename, 1, 1);
        } else {
          Module['addRunDependency']("fp " + entries[i].filename);
          entries[i].getData(
            new zip.ArrayBufferWriter(), 
            (function(entry) {
              return function(data) {
                Module['FS_createPreloadedFile']("/" + entry.filename, null, new Uint8Array(data), true, true,
                  function() { // ondone
                    //console.log("Preloaded " + entry.filename);
                    Module['removeRunDependency']("fp " + entry.filename);
                  },
                  function() { // onerror
                    console.log("OH NO EERROR");
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

var xhr = new XMLHttpRequest();
xhr.open('GET', '/zip.js', true);
xhr.onload=function() {
  eval.call(window, xhr.response);
  xhr.open('GET', '/zip-ext.js', true);
  xhr.onload=function() {
    eval.call(window, xhr.response);
    xhr.open('GET', '/motor2d.js', true);
    xhr.onload=function() {
      if (Module['calledRun']) {
        runWithFS();
      } else {
        if (!Module['preRun']) Module['preRun'] = [];
        Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
      }  
      eval.call(window, xhr.response);
    }
    xhr.send(null);
  }
  xhr.send(null);
}
xhr.send(null);


