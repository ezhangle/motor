#!/bin/bash

cat <<'EOF'
<!doctype html>
<html lang="en-us">
<head>
  <meta charset="utf-8">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">

  <style>
    body {
      font-family: sans-serif;
      width: 50%;
    }
    .yes {
      background-color: #A4C75C;
    }
    .no {
      background-color: #CC8B70;
    }
    .partial {
      background-color: #E3DB73;
    }
    .header {
      font-weight: bold;
    }
  </style>
  <title>
  Motor2D - Bringing Löve2D to the Web
  </title>
</head>
<body>

<img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAIQAAAA0BAMAAAC5oI3hAAAAMFBMVEWjQ0JoaWdwcm9/gX6MjovCgX+dn5y3ubbfurvNz8vX2dbz3uHq5+Xw8u/2+PT9//wYI7m7AAAAAWJLR0QAiAUdSAAAAAlwSFlzAAALEwAACxMBAJqcGAAAAAd0SU1FB98BEBEAH9IpvscAAAORSURBVEjHxdZNaBNBFAfwuRSEEkoVWlpqexD0pvTSnrQg4kloLoWASOgHQk+h4sWL7aXnUA96DNFbqTQXQYhlaW+KMfUk1IbFevBmgv1KNjsZZ9682XmTrBoVcQ/Z+bs7v8y+zL7K8ueFOYL+qviDg+UHovFJV0Q9yXqyLjHrm/H2rEN8iAWbLFVcZZ5D7GTMeD7vTMrHEit35Mdxr0N8GcFhONQFwXvglPQp8fWiKcVUF8RxH5x2s5SoLpWxFLkuiFpaS2mHOMBizIsuiIL++nqfQzRGsBR6Uuvx+IVFIPr7cyQGo637E6LgGaLGWM9MygNCTGApNPFscuP13C1FlCpVEoPRR4vvxAs/IuA84wGxtqlLAURjUNV9rowPYmMwNET2FxKi3gvE4bQKC3rSzjIs6RoSNgZnc05Va1gcXxHBsCrFsJ50Ay60JpGwMRigGyxriHqamWkn0zApwI22VgWCxIA8R1NuMCR4Aoi8LMb7TSBkUeE40JUhMbCvtNhNRIRIAaFWsKB3wuEybtUMECQGo5ZIepZYBULWgQ9r4hPW7FT/xCQSAt4yQxSAkEtQC1GTtnG3N64AQSIhkllC7GpiO6dKoSbl8RVsjAFBoiWOzwhC1DRxMnVXuIScQAkZLbGSjiH44Ihoe5Ax90HIKuq6ZbQRYimDBDyQqp+uBYkRUUg7BJZTfC4j8Q1/AlledYVEQzSZcIhVRruCHB9h85AbQl0h0RCFhEuk2onTy9jCNoEgEQluurchetuJ1iXTwoAgEYmoebuvGW1196Aq6p2CKzYikcy6RMHrII6uqtHTDBI2akJvK0tgy3GI1vWH8g/ZgGm/Nmpi5XYRDk8TL7HxuT37dPzm/Llc1MGjCESd4ZFQ7ZcxaL+dfT548qD84xjzl1389RFD8P3ST2IMsUcCjMP1jTfrvkk2cvW68GLRlLOpTluxhLo/3IiIKAIhPkZ1acISizFECHfulc2aoqiJcMshBPc6CZ35cxzZqAlRdAl56iCe6+FbnG0jEk3PJXi5neD4rIEPBIlIRMtAQnhsnxByHOKc0FeJRqOZZRiiwkJ9i/kMfbM++DcShfktXrkER2LfN4RwCdFJNP1YolTunhAlsvCI4H6ljRAuISjByaciuI54ii4IH4YkCvtfTT+GqMh1/AbBYwi8oVvC3IxvKtzoCzOB2y+iBBcO4b7s5kbnO90F/Yr4J13rPxDfAew1TNBhkvCaAAAAAElFTkSuQmCC">
<h1>Bringing Löve2D to the Web</h1>
<h2>What is this?</h2>

<p>
<a href="http://love2d.org">Löve2D</a> is an awesome framework that provides an OpenGL backed 2D game engine programmable in the <a href="http://lua.org">Lua</a> scripting language.
</p>

<p> A lot of great games have been made using Löve2D and the number is steadily
growing. The only downside of Löve2D is that it cannot be embedded in websites
for people to play.  This is what Motor2D is trying to solve: A version of
Löve2D that can run on any modern WebGL-enabled browser.  </p>

<h2>How is it done?</h2>

<p>
Motor2D is a reimplementation of Löve2D's Lua interface in C, which is then
compiled to JavaScript using the unbelievable <a href="http://kripken.github.io/emscripten-site">emscripten</a> toolset.  </p>

<h2>Why a Reimplementation Instead of a Port?</h2>

<p>
In fact, there are several reasons for this. The most important one is that I just wanted to do it like this and it just felt right. But there are also some more rational thoughts:
</p>

<ul>
<li>
  <strong>Löve2D is written in C++ and makes extensive use of exceptions.</strong> While there is absolutely no problem with this on real computers, exceptions are very costly to implement in asm.js (the highly optimizable subset of JavaScript that emscripten compiles to), even if no exception is thrown at all. So all error handling would either have to be slowing down the whole framework, rewritten or stripped out, neither of which was an option.
</li>
<li>
  <strong>Löve2D requires some external libraries that are not readily available on emscripten.</strong> Löve2D is built around SDL2, which in itself is a great library, but is not included in emscripten (only through emscripten ports) and adds a bit of extra weight to the final executable. I also couldn't find information an how much browser functionality is reused by SDL2, while emscripten's SDL1 implementation tries to use the browsers built in functionality for as much stuff as possible (image loading for example). The same goes for PhysicsFS.
</li>
<li>
  <strong>Emscripten brings its own virtual filesystem</strong>, so using Löve's integrated PhysicsFS is not only unnecessary, it also cannot provide preloading of image data.
</li>
<li>
  <strong>I feel much more comfortable using "dirty" hacks or shortcuts in C than in C++.</strong> Code size and execution speed are very important when running on the browser, and C++ code usually tends to ask for a much cleaner design than C. I admit that this may just be my personal opinion, but in C++ I tend to "feel bad" about every typecast in the code, especially if it's from one type to another, technically unrelated type. In C, this is often the only way to achieve certain things, so I feel much more comfortable doing it.
</li>
<li>
  <strong>Löve2D aims to be compatible to old hardware</strong> and therefore uses a lot of runtime polymorphy to select the correct implementation of certain things for several different OpenGL versions. While this is fine and a good thing on real hardware, on the web there is really only WebGL 1.0 to care about at the moment. So all runtime selectable implementation stuff is just useless overhead for a system that is only going to support one single target platform.
</li>
</ul>

<p> All these points together felt like a good reason to use C for this project
and reimplement everything. When developing for a "real computer" instead of a
browser, I wouldn't care too much about the slight overhead of a virtual
function call to the graphics system, but even though emscripten generates
incredibly fast JavaScript, it is still JavaScript. Also, Löve2D uses LuaJIT,
an equally incredibly fast Lua VM. But we can't use it in the browser, so we
are stuck with plain old vanilla Lua, which is still a pretty fast scripting
language and VM, but several times slower than LuaJIT. Therefore, every bit of
time that can be saved is good thing. Motor2D will be slow enough anyway.
</p>

<h2>Usage</h2>

There are two ways to use the framework: Either with the development server included in the package or by putting it on a webserver. Please note that the framework <strong>cannot</strong> be served from a local filesystem, because it uses XMLHttpRequests to load the game file (.love) and engine.

<h3>Direct Usage on Webserver</h3>

Copy the files mentionen in section Building to your webserver.
The file motor2d.html may adopted to your needs or completely replaced and renamed, as long as it has an empty <div> element with id "motor_output" and it loads motor2dloader.js.

Put your .love file into the same directory and call it game.love. When opening motor2d.html in a browser, it should load and start your game.

You can use a different name for the .love file if you open the page with query option motor2d.html?love=yourfilename.love

<h3>Using the Built-In Development Server</h3>

The development server has one big advantage: It saves you from having to package your .love file before each run (as long as the filename remains game.love).

To use it, do the following: Add the motor2d base directory to your PATH environment variable:

<pre>
  # bash syntax:
  gamedir$ export PATH=&lt;motor2ddirectory&gt;:$PATH
</pre>

When your Lua source files and data files are in the directory gamedir, you can just start "motor serve" in this directory:

<pre>
  gamedir$ motor serve
</pre>

Whenever /game.love is requested by the web browser, the whole gamedir will be zipped into the file _build/game.love and served from there.

<h2>Performance</h2>
It is naturally quite a bit slower than the original, but many games work with acceptable speed.

<h2>Compatibility</h2>
<p>
The following table gives an overview of the availability of some of Löve2D's functions and classes in Motor2D. Classes are left out if support for them is not available at all (especially if the appropriate newXY function is still missing).
</p>

<p>
Green functions are already available and should work as expected. Yellow functions are there but may behave differently or be only partially implemented. In this case, the third column of the table sums up the differences. Red functions are missing completely.
</p>

<p>
I am aiming for 100% script compatibility between Löve2D and Motor2D, but it is still a lot of work to get there and even then some minor incompatibilities will not be avoidable.
</p>

<p>
Error handling is pretty bare bones so far. If something crashes, check your browsers web console, if the error message is unusable, sorry, I guess. I promise to improve it, but I am focussing on other parts right now.

<table>
<tr class="header"><td>Element</td><td>Vers.</td><td>Notes </td></tr>
EOF

awk -F'\t' '{printf("<tr class=\"%s\"><td>%s</td><td>%s</td><td>%s</td></tr>\n", $2, $1, $2, $3);}' <functions2.csv


cat <<EOF
</table>

<h2>Open Source?</h2>
Q: Is Motor2d open source?<br>
A: Not yet, but I promise it will be. At the moment I want to do more of the work myself and get everything in a state where I don't have to worry about people complaining about the code quality. I try to keep everything clean already, but some parts of the code are necessarily a bit messy, because they are just there to allow other parts to function. I promise to open it under the MIT license in the near future, for now it is closed source though.

<h2>Known Problems</h2>
Lots of them. There are many incompatible functions. There's also a bug in the development server. When you first run the server on a clean Löve project, and try to open it in a browser, you will get an error message, that game.love was not found. Just restart the server and reload the page.

<h2>Downloads</h2>
<a href="motor2d-0.0.1-preview.tar.bz2">motor2d-0.0.1-preview.tar.bz2</a><br>
<a href="motor2d-0.0.1-preview.zip">motor2d-0.0.1-preview.zip</a>

<h2>Legal Stuff</h2>
You may use Motor2d any way you want. <br>

<p>
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.</p>

<p>Copyright &copy; 2015 Florian Keßeler</p>

<h2>Contact</h2>
If you have any bug reports, suggestions or whatever, you can contact me at flo at base13 de.
</body>
</html>
EOF
