var engineReq = new XMLHttpRequest();
var gameReq = new XMLHttpRequest();

function loadGame() {
  Module['addRunDependency']("zip game.love");
  zip.workerScripts = {
    inflater: ['z-worker.js', 'inflate.js']
  };

  zip.createReader(new zip.BlobReader(gameReq.response), function(reader) {
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

var Module = {
  preRun: [loadGame],
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
  TOTAL_MEMORY:67108864
};


var enginesize, gamesize;
var engineloaded = 0, gameloaded = 0
function updateProgress() {
  var ratio = Math.floor(100 *(engineloaded + gameloaded) / (enginesize + gamesize))
  engine_progress.style.width = (4*ratio) + "px";
  engine_progress3.innerHTML = ratio + "% loaded";
}

var layer = document.getElementById('motor_output');
var canvas;
var engine_progress, engine_progress2, engine_progress3
if(!layer) {
  document.body.innerHTML = "<img src=\"motor.png\"><br>Could not find motor_output layer. Please add &lt;div id=\"motor_output\"&gt;&lt;/div&gt; in the appropriate place in your HTML";

} else {

  var img = document.createElement('img');
  img.src="motor.png";
  layer.appendChild(img);
  var green = "#BF3D3D";
  var red = "#eee";
  engine_progress = document.createElement('div');
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

  engine_progress2 = document.createElement('div');
  engine_progress2.style.width="400px";
  engine_progress2.style.height="15px";
  engine_progress2.style.marginLeft="auto";
  engine_progress2.style.marginRight="auto";
  engine_progress2.style.marginTop = "2px";
  engine_progress2.style.backgroundColor = red;
  layer.appendChild(engine_progress2);
  engine_progress2.appendChild(engine_progress);

  engine_progress3 = document.createElement('div');
  engine_progress3.style.align="center";
  engine_progress3.style.marginTop="5px";
  engine_progress3.style.fontSize="12px";
  engine_progress3.style.color = "#888";
  layer.appendChild(engine_progress3);
}


function bootEngine() {
  if(engineReq.readyState == 4 && gameReq.readyState == 4) {
    while(layer.firstChild) {
      layer.removeChild(layer.firstChild);
    }
    canvas = document.createElement("canvas");
    canvas.addEventListener("contextmenu", function(event) {event.preventDefault();}, false);
    canvas.id="motor_canvas";
    layer.appendChild(canvas);
    Module.canvas = canvas;
    canvas.addEventListener("webglcontextlost", function(e) { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);
    layer.appendChild(canvas);
    eval.call(window, engineReq.response);
  }
}


function loadEngine() {
  if(gamesize && enginesize) {
    engineReq.open('GET', 'motor2d.js', true);
    engineReq.onload = function() {
      //eval.call(window, engineReq.response);
      console.log("engine loaded");
      bootEngine();
    };
    engineReq.onprogress = function(a) {
      engineloaded = a.loaded;
      updateProgress();
    };
    engineReq.send();
    gameReq.open('GET', 'game.love', true);
    gameReq.onload = function() {
      console.log("game loaded");
      bootEngine();
    };
    gameReq.responseType = "blob";
    gameReq.onprogress = function(a) {
      gameloaded = a.loaded;
      updateProgress();
    };
    gameReq.send();
  }
}

engineReq.open('HEAD', 'motor2d.js', true);
engineReq.onload = function() {
  enginesize = parseInt(engineReq.getResponseHeader('Content-Length'));
  loadEngine();
};
engineReq.send();

gameReq.open('HEAD', 'game.love', true);
gameReq.onload = function() {
  gamesize = parseInt(gameReq.getResponseHeader('Content-Length'));
  loadEngine();
};
gameReq.send();
