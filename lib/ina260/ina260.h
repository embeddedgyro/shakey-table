#ifndef INA260_H
#define INA260_H
#include "i2c_interface.h"
#include <thread>

namespace INA260_Driver {
  enum class alert_conf{
    OCL = 0b1000000000000000,
    UCL = 0b0100000000000000,
    BOL = 0b0010000000000000,
    BUL = 0b0001000000000000
    POL = 0b0000100000000000,
    CNVR = 0b0000010000000000
}
}

#endif
