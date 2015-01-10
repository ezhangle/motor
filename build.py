#!/usr/bin/python

import sys
import os
import re
import shutil

sources = [
  'graphics/batch.c',
  'graphics/canvas.c',
  'graphics/font.c',
  'graphics/graphics.c',
  'graphics/image.c',
  'graphics/matrixstack.c',
  'graphics/quad.c',
  'graphics/shader.c',
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
  'tools/utf8.c',

  # Lua
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

  # FreeType
#  '3rdparty/freetype/src/autofit/autofit.c',
#  '3rdparty/freetype/src/base/ftadvanc.c',
#  '3rdparty/freetype/src/base/ftbbox.c',
#  '3rdparty/freetype/src/base/ftbdf.c',
  '3rdparty/freetype/src/base/ftbitmap.c',
  '3rdparty/freetype/src/base/ftcalc.c',
#  '3rdparty/freetype/src/base/ftcid.c',
#  '3rdparty/freetype/src/base/ftdbgmem.c',
#  '3rdparty/freetype/src/base/ftdebug.c',
#  '3rdparty/freetype/src/base/ftfstype.c',
#  '3rdparty/freetype/src/base/ftgasp.c',
  '3rdparty/freetype/src/base/ftgloadr.c',
  '3rdparty/freetype/src/base/ftglyph.c',
#  '3rdparty/freetype/src/base/ftgxval.c',
  '3rdparty/freetype/src/base/ftinit.c',
#  '3rdparty/freetype/src/base/ftlcdfil.c',
#  '3rdparty/freetype/src/base/ftmm.c',
  '3rdparty/freetype/src/base/ftobjs.c',
#  '3rdparty/freetype/src/base/ftotval.c',
  '3rdparty/freetype/src/base/ftoutln.c',
#  '3rdparty/freetype/src/base/ftpatent.c',
#  '3rdparty/freetype/src/base/ftpfr.c',
  '3rdparty/freetype/src/base/ftrfork.c',
#  '3rdparty/freetype/src/base/ftsnames.c',
  '3rdparty/freetype/src/base/ftstream.c',
#  '3rdparty/freetype/src/base/ftstroke.c',
#  '3rdparty/freetype/src/base/ftsynth.c',
  '3rdparty/freetype/src/base/ftsystem.c',
  '3rdparty/freetype/src/base/fttrigon.c',
#  '3rdparty/freetype/src/base/fttype1.c',
  '3rdparty/freetype/src/base/ftutil.c',
#  '3rdparty/freetype/src/base/ftwinfnt.c',
#  '3rdparty/freetype/src/base/ftxf86.c',
#  '3rdparty/freetype/src/bdf/bdf.c',
#  '3rdparty/freetype/src/bzip2/ftbzip2.c',
#  '3rdparty/freetype/src/cache/ftcache.c',
#  '3rdparty/freetype/src/cff/cff.c',
#  '3rdparty/freetype/src/cid/type1cid.c',
  '3rdparty/freetype/src/gzip/ftgzip.c',
#  '3rdparty/freetype/src/lzw/ftlzw.c',
#  '3rdparty/freetype/src/pcf/pcf.c',
#  '3rdparty/freetype/src/pfr/pfr.c',
#  '3rdparty/freetype/src/psaux/psaux.c',
#  '3rdparty/freetype/src/pshinter/pshinter.c',
#  '3rdparty/freetype/src/psnames/psmodule.c',
#  '3rdparty/freetype/src/raster/raster.c',
  '3rdparty/freetype/src/sfnt/sfnt.c',
  '3rdparty/freetype/src/smooth/smooth.c',
  '3rdparty/freetype/src/truetype/truetype.c',
#  '3rdparty/freetype/src/type1/type1.c',
#  '3rdparty/freetype/src/type42/type42.c',
#  '3rdparty/freetype/src/winfonts/winfnt.c'
]

output = 'motor2d.js'


SRCDIR = os.path.dirname(sys.argv[0]) + "/src"

ftinc = " ".join(map(lambda x: "-I" + os.path.relpath(SRCDIR) + "/3rdparty/freetype/src/" + x, ["truetype", "sfnt", "autofit", "smooth", "raster", "psaux", "psnames"])) + " -I" + os.path.relpath(SRCDIR) + "/3rdparty/freetype/include"

CFLAGS = '-DFT2_BUILD_LIBRARY -Wall -std=c11 -O2 --llvm-lto 3 -I{ftconfig}  -I{srcdir}/3rdparty/lua/src'.format(srcdir = os.path.relpath(SRCDIR), ftconfig=".") + " " + ftinc
CC = 'emcc'
LD = 'emcc'
LDFLAGS = '-O2 --llvm-lto 3'

if SRCDIR == '.' or SRCDIR == '':
  print("Please build out-of-source")
  sys.exit(1)

includeregex = re.compile('^\s*#\s*include\s*"([^"]+)"\s*$')

os.system('sed -e "s/FT_CONFIG_OPTIONS_H/<ftoption.h>/" -e "s/FT_CONFIG_STANDARD_LIBRARY_H/<ftstdlib.h>/" -e "s?/undef ?#undef ?" <{srcdir}/3rdparty/freetype/builds/unix/ftconfig.in >ftconfig.h'.format(srcdir=os.path.relpath(SRCDIR)))
os.system('mkdir -p config; sed -e \'/tt_driver\\|sfnt_module\\|ft_smooth_renderer/ !d\' < {srcdir}/3rdparty/freetype/include/config/ftmodule.h >config/ftmodule.h'.format(srcdir=os.path.relpath(SRCDIR)))


def newestDependency(filename, trace=[]):
  newest = os.path.getmtime(sys.argv[0])
  with open(filename) as f:
    for line in f:
      res = includeregex.match(line)
      if res:
        dep = os.path.dirname(filename) + "/" + res.group(1)
        if os.path.exists(dep) and dep not in trace:
          newest = max(newest, os.path.getmtime(dep), newestDependency(dep, trace + [dep]))

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

