#!/usr/bin/python

import sys
import os

sources = [
  'main.c'
]

output = 'motor2d.html'

CFLAGS = '-Wall -std=c99 -s USE_SDL=2'
CC = 'emcc'
LD = 'emcc'
LDFLAGS = '-s USE_SDL=2'

SRCDIR = os.path.dirname(sys.argv[0])

def needsRebuild(filename):
  return not os.path.exists(getObjFilename(filename)) or \
    os.path.getmtime(filename) > os.path.getmtime(getObjFilename(filename)) or \
    os.path.getmtime(sys.argv[0]) > os.path.getmtime(output)


def getObjFilename(filename):
  return os.path.splitext(os.path.basename(filename))[0] + ".o"


def compile(filename):
  objfile = getObjFilename(filename)
  print("Compile {filename} -> {objfile}".format(filename=filename, objfile=objfile))
  cmd = "{CC} {CFLAGS} -c -o {objfile} {filename}".format(CC=CC, CFLAGS=CFLAGS, filename=filename, objfile=objfile)
  return os.system(cmd) == 0

def build():
  needsLinking = False
  for i in sources:
    filename = SRCDIR + "/" + i
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


build()

