#define BOOST_TEST_MODULE mpu6050Test
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <cassert>
#include <cmath>
#include <limits>
#include <thread>
#include <chrono>
#include <iostream>
#include "../../lib/pid/pid.h"
#include "../../lib/mpu6050/mpu6050.h"
#include "../../lib/i2c_interface/smbus_i2c_if.h"
#include "../../lib/ina260/ina260.h"
#include "../../lib/MotorDriver/MotorDriver.h"

