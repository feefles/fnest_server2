#ifndef MMAP_HPP
#define MMAP_HPP

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

/* assert(/tmp mounted to tmpfs, i.e. resides in RAM) */
/* just use any file in /tmp */
static const char* mmapFilePath = "/tmp/arduino";


typedef struct mmapData_t {
  int cur_temp;   // led on IO8
  int set_temp;  // built-in led
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} mmap_Data;

#endif
