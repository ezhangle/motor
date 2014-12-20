#include "motor.h"

motor_Version const * motor_getVersion() {
  static const motor_Version version = {
    .major = 0,
    .minor = 0,
    .revision = 1,
    .codename = "Dead Opossum"
  };

  return &version;
}
