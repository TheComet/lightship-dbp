//
// Copyright 2011-2014 NimbusKit
// Originally ported from https://github.com/ingenuitas/python-tesseract/blob/master/fmemopen.c
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "util/pstdint.h"

struct fmem {
  uintptr_t pos;
  uintptr_t size;
  char *buffer;
};
typedef struct fmem fmem_t;

static int readfn(void *handler, char *buf, uintptr_t size) {
  fmem_t *mem = handler;
  uintptr_t available = mem->size - mem->pos;

  if (size > available) {
	size = available;
  }
  memcpy(buf, mem->buffer + mem->pos, sizeof(char) * size);
  mem->pos += size;

  return size;
}

static int writefn(void *handler, const char *buf, uintptr_t size) {
  fmem_t *mem = handler;
  uintptr_t available = mem->size - mem->pos;

  if (size > available) {
	size = available;
  }
  memcpy(mem->buffer + mem->pos, buf, sizeof(char) * size);
  mem->pos += size;

  return size;
}

static fpos_t seekfn(void *handler, fpos_t offset, uintptr_t whence) {
  uintptr_t pos;
  fmem_t *mem = handler;

  switch (whence) {
	case SEEK_SET: {
	  if (offset >= 0) {
		pos = (uintptr_t)offset;
	  } else {
		pos = 0;
	  }
	  break;
	}
	case SEEK_CUR: {
	  if (offset >= 0 || (uintptr_t)(-offset) <= mem->pos) {
		pos = mem->pos + (uintptr_t)offset;
	  } else {
		pos = 0;
	  }
	  break;
	}
	case SEEK_END: pos = mem->size + (uintptr_t)offset; break;
	default: return -1;
  }

  if (pos > mem->size) {
	return -1;
  }

  mem->pos = pos;
  return (fpos_t)pos;
}

static int closefn(void *handler) {
  free(handler);
  return 0;
}

FILE *fmemopen(void *buf, uintptr_t size, const char *mode) {
  // This data is released on fclose.
  fmem_t* mem = (fmem_t *) malloc(sizeof(fmem_t));

  // Zero-out the structure.
  memset(mem, 0, sizeof(fmem_t));

  mem->size = size;
  mem->buffer = buf;

  // funopen's man page: https://developer.apple.com/library/mac/#documentation/Darwin/Reference/ManPages/man3/funopen.3.html
  return funopen(mem, readfn, writefn, seekfn, closefn);
}
