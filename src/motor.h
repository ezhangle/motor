#pragma once

typedef struct {
  int major;
  int minor;
  int revision;
  char const * codename;
} motor_Version;

motor_Version const * motor_getVersion();
