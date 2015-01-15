#include "love_file.h"
// TODO Cconfigure miniz defines
#define _LARGEFILE64_SOURCE 1
#define _LARGEFILE_SOURCE 1
#define _POSIX_SOURCE 1
#define MINIZ_HAS_64BIT_REGISTERS 0
#include "3rdparty/miniz/miniz.c"
#include <emscripten.h>

static mz_zip_archive archive;
static int i = 0;
static char *filename = NULL;
static int curfilenamelen = 0;
static void const* inputbuffer;
void launch_game();

void unzip_error(char const* f) {
  printf("Error prepareing %s\n", f);
}

void unzip_and_prepare(char* f) {
  printf("here\n");
  if(i == mz_zip_reader_get_num_files(&archive)) {
    free(inputbuffer);
    free(filename);
    launch_game();
    return;
  }

  int newfnamelen = mz_zip_reader_get_filename(&archive, i, NULL, 0);

  // round up
  newfnamelen--;
  newfnamelen |= newfnamelen >> 1;
  newfnamelen |= newfnamelen >> 2;
  newfnamelen |= newfnamelen >> 4;
  newfnamelen |= newfnamelen >> 8;
  newfnamelen |= newfnamelen >> 16;
  newfnamelen++;

  if(newfnamelen > curfilenamelen) {
    free(filename);
    filename = malloc(newfnamelen);
    curfilenamelen = newfnamelen;
  }

  mz_zip_reader_get_filename(&archive, i, filename, curfilenamelen);
  
  if(mz_zip_reader_is_file_a_directory(&archive, i)) {
    mkdir(filename, 0777);
  } else {
    mz_zip_reader_extract_to_file(&archive, i, filename, 0);
  }
  ++i;
  printf("unzipped %s\n", filename);

  emscripten_async_prepare(filename, unzip_and_prepare, unzip_error);

}

int unzip_love_file(void const* buffer, int size) {
  inputbuffer = buffer;
  mz_zip_reader_init_mem(&archive, buffer, size, MZ_ZIP_FLAG_CASE_SENSITIVE);
  unzip_and_prepare(NULL);
}
