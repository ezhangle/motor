#!/usr/bin/python

import sys
import os
import re
import shutil

sources = [
  'main.c',
  'lua/src/lapi.c',
  'lua/src/lauxlib.c',
  'lua/src/lbaselib.c',
  'lua/src/lbitlib.c',
  'lua/src/lcode.c',
  'lua/src/lcorolib.c',
  'lua/src/lctype.c',
  'lua/src/ldblib.c',
  'lua/src/ldebug.c',
  'lua/src/ldo.c',
  'lua/src/ldump.c',
  'lua/src/lfunc.c',
  'lua/src/lgc.c',
  'lua/src/linit.c',
  'lua/src/liolib.c',
  'lua/src/llex.c',
  'lua/src/lmathlib.c',
  'lua/src/lmem.c',
  'lua/src/loadlib.c',
  'lua/src/lobject.c',
  'lua/src/lopcodes.c',
  'lua/src/loslib.c',
  'lua/src/lparser.c',
  'lua/src/lstate.c',
  'lua/src/lstring.c',
  'lua/src/lstrlib.c',
  'lua/src/ltable.c',
  'lua/src/ltablib.c',
  'lua/src/ltm.c',
  'lua/src/lundump.c',
  'lua/src/lvm.c',
  'lua/src/lzio.c',
]

output = 'motor2d.js'
extra_output = 'motor2d.html'

CFLAGS = '-Wall -std=c99 -s USE_SDL=2 -s ASYNCIFY=1'
CC = 'emcc'
LD = 'emcc'
LDFLAGS = '-s USE_SDL=2 -s ASYNCIFY=1'

SRCDIR = os.path.dirname(sys.argv[0])

if SRCDIR == '.' or SRCDIR == '':
  print("Please build out-of-source")
  sys.exit(1)

includeregex = re.compile('^\s*#\s*include\s*"([^"]+)"\s*$')

def newestDependency(filename):
  newest = 0
  with open(filename) as f:
    for line in f:
      res = includeregex.match(line)
      if res:
        dep = os.path.dirname(filename) + "/" + res.group(1)
        newest = max(newest, os.path.getmtime(dep), newestDependency(dep))

  return newest



def needsRebuild(filename):
  return not os.path.exists(getObjFilename(filename)) or \
    os.path.getmtime(SRCDIR + "/" + filename) > os.path.getmtime(getObjFilename(filename)) or \
    os.path.getmtime(sys.argv[0]) > os.path.getmtime(output) or newestDependency(SRCDIR + "/" + filename) > os.path.getmtime(getObjFilename(filename))


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

  shutil.copyfile(SRCDIR + "/" + extra_output, extra_output)

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

