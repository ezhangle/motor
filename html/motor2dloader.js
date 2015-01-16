var Module = {
  preRun: [],
  postRun: [],
  print: (function() {
    return function(text) {
      text = Array.prototype.slice.call(arguments).join(' ');
      console.log(text);
    };
  })(),
  printErr: function(text) {
    text = Array.prototype.slice.call(arguments).join(' ');
    console.error(text);
  },
};

Module.TOTAL_MEMORY=67108864;

function runWithFS() {
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
xhr.onreadystatechange = function (evt) {
  if(xhr.readyState === 4) {  
    if(xhr.status != 200) {  
      var text = "Failed to load a component (" + xhr.responseURL + "): " + xhr.statusText;  
      console.log(text);
      document.body.innerHTML = text;
      xhr.abort();
    }  
  } 
};



xhr.open('HEAD', 'motor2d.js', true);
xhr.onload=function() {
  window.enginesize = parseInt(xhr.getResponseHeader('Content-Length'));

  xhr.open('HEAD', 'game.love', true);
  xhr.onload=function() {
    window.gamesize = parseInt(xhr.getResponseHeader('Content-Length'));

    window.setProgress = function(action, done, total) {
      
    }

    window.mkProgress = function(a,b){}

    var layer = document.getElementById('motor_output');
    if(layer) {
      var img = document.createElement('img');
      img.src="/motor.png";
      layer.appendChild(img);
      var green = "#BF3D3D";
      var red = "#eee";
      var engine_progress = document.createElement('div');
      engine_progress.style.backgroundColor = green;
      engine_progress.style.width="0px";
      engine_progress.style.paddingLeft="5px";
      engine_progress.style.textAlign="left";
      engine_progress.style.overflow="hidden";
      engine_progress.style.whiteSpace ="nowrap";
      engine_progress.style.color = "#692828";
      engine_progress.style.fontFamily = "arial";
      engine_progress.style.fontSize = "12px";
      engine_progress.style.height = "15px";

      var engine_progress2 = document.createElement('div');
      engine_progress2.style.width="400px";
      engine_progress2.style.height="15px";
      engine_progress2.style.marginLeft="auto";
      engine_progress2.style.marginRight="auto";
      engine_progress2.style.marginTop = "2px";
      engine_progress2.style.backgroundColor = red;
      layer.appendChild(engine_progress2);
      engine_progress2.appendChild(engine_progress);

      var engine_progress3 = document.createElement('div');
      engine_progress3.style.align="center";
      engine_progress3.style.marginTop="5px";
      engine_progress3.style.fontSize="12px";
      engine_progress3.style.color = "#888";
      layer.appendChild(engine_progress3);

      window.mkProgress = function(title, scale, offset, doneAction) {
        window.setProgress = function(done, total) {
          var d = Math.floor(100 * (offset + scale * done / total));
          engine_progress3.innerHTML = title + " (" + d + "%)";
          engine_progress.style.width = (4*d) + "px";
          if(done == total && doneAction) {
            doneAction();
          }
        }
      }

      var scale = window.enginesize / (window.enginesize + window.gamesize);
      console.log(scale);
      mkProgress("Loading Engine", scale, 0.0, function() {
        mkProgress("Loading Game", 1-scale, scale, function() {
          while(layer.firstChild) {
            layer.removeChild(layer.firstChild);
          }
          var canvas = document.createElement("canvas");
          canvas.addEventListener("contextmenu", function(event) {event.preventDefault();}, false);
          canvas.id="motor_canvas";
          layer.appendChild(canvas);
          Module.canvas = canvas;
          canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
          layer.appendChild(canvas);
        });
        setProgress(0, 1);
      });

      setProgress(0, 1);

    }
    xhr.open('GET', '/zip.js', true);
    xhr.onload=function() {
      eval.call(window, xhr.response);
      xhr.open('GET', '/zip-ext.js', true);
      xhr.onload=function() {
        eval.call(window, xhr.response);
        xhr.open('GET', '/motor2d.js', true);
        xhr.onprogress = function(a) {
          setProgress(a.loaded, a.total);
        };
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
  }
  xhr.send(null);
}
xhr.send(null);

