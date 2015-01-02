#!/usr/bin/python

import sys
import os
import re
import shutil

sources = [
  'graphics/graphics.c',
  'graphics/image.c',
  'graphics/matrixstack.c',
  'graphics/quad.c',
  'image/imagedata.c',
  'luaapi/boot.c',
  'luaapi/keyboard.c',
  'luaapi/motor.c',
  'luaapi/graphics.c',
  'luaapi/image.c',
  'luaapi/tools.c',
  'math/vector.c',
  'main.c',
  'motor.c',
  'keyboard.c',
  '3rdparty/lua/src/lapi.c',
  '3rdparty/lua/src/lauxlib.c',
  '3rdparty/lua/src/lbaselib.c',
  '3rdparty/lua/src/lbitlib.c',
  '3rdparty/lua/src/lcode.c',
  '3rdparty/lua/src/lcorolib.c',
  '3rdparty/lua/src/lctype.c',
  '3rdparty/lua/src/ldblib.c',
  '3rdparty/lua/src/ldebug.c',
  '3rdparty/lua/src/ldo.c',
  '3rdparty/lua/src/ldump.c',
  '3rdparty/lua/src/lfunc.c',
  '3rdparty/lua/src/lgc.c',
  '3rdparty/lua/src/linit.c',
  '3rdparty/lua/src/liolib.c',
  '3rdparty/lua/src/llex.c',
  '3rdparty/lua/src/lmathlib.c',
  '3rdparty/lua/src/lmem.c',
  '3rdparty/lua/src/loadlib.c',
  '3rdparty/lua/src/lobject.c',
  '3rdparty/lua/src/lopcodes.c',
  '3rdparty/lua/src/loslib.c',
  '3rdparty/lua/src/lparser.c',
  '3rdparty/lua/src/lstate.c',
  '3rdparty/lua/src/lstring.c',
  '3rdparty/lua/src/lstrlib.c',
  '3rdparty/lua/src/ltable.c',
  '3rdparty/lua/src/ltablib.c',
  '3rdparty/lua/src/ltm.c',
  '3rdparty/lua/src/lundump.c',
  '3rdparty/lua/src/lvm.c',
  '3rdparty/lua/src/lzio.c',
]

output = 'motor2d.js'


SRCDIR = os.path.dirname(sys.argv[0]) + "/src"

CFLAGS = '-Wall -std=c99 -I{srcdir}/3rdparty/lua/src'.format(srcdir = os.path.relpath(SRCDIR))
CC = 'emcc'
LD = 'emcc'
LDFLAGS = ''

if SRCDIR == '.' or SRCDIR == '':
  print("Please build out-of-source")
  sys.exit(1)

includeregex = re.compile('^\s*#\s*include\s*"([^"]+)"\s*$')

def newestDependency(filename):
  newest = os.path.getmtime(sys.argv[0])
  with open(filename) as f:
    for line in f:
      res = includeregex.match(line)
      if res:
        dep = os.path.dirname(filename) + "/" + res.group(1)
        if os.path.exists(dep):
          newest = max(newest, os.path.getmtime(dep), newestDependency(dep))

  return newest



def needsRebuild(filename):
  return not os.path.exists(getObjFilename(filename)) or \
    os.path.getmtime(SRCDIR + "/" + filename) > os.path.getmtime(getObjFilename(filename)) or \
    newestDependency(SRCDIR + "/" + filename) > os.path.getmtime(getObjFilename(filename))


def getObjFilename(filename):
  return os.path.splitext(filename)[0] + ".o"


def compile(filename):
  objfile = getObjFilename(filename)
  outdir = os.path.dirname(filename)
  if outdir == "":
    outdir = "."
  if not os.path.exists(outdir):
    print("Making directory " + outdir)
    os.makedirs(outdir)

  print("Compile {filename} -> {objfile}".format(filename=filename, objfile=objfile))
  cmd = "{CC} {CFLAGS} -c -o {objfile} {filename}".format(CC=CC, CFLAGS=CFLAGS, filename=SRCDIR + "/" + filename, objfile=objfile)
  return os.system(cmd) == 0

def build():
  needsLinking = False
  for i in sources:
    filename = i
    if needsRebuild(filename):
      if not compile(filename):
        print("Failed")
        sys.exit(1)

      needsLinking = True

  if needsLinking:
    print("Linking {output}".format(output=output))
    cmd = "{LD} {LDFLAGS} -o {outfile} {infiles}".format(LD=LD, LDFLAGS=LDFLAGS, outfile=output, infiles=" ".join(map(getObjFilename, sources)))
    if os.system(cmd) != 0:
      print("Failed")

def remove(f):
  if os.path.exists(f):
    os.remove(f)

def clean():
  for i in sources:
    remove(getObjFilename(i))

  remove(output)

  for i in extra_output:
    remove(extra_output)


if len(sys.argv) == 1 or sys.argv[1] == 'build':
  build()
elif sys.argv[1] == 'clean':
  clean()

