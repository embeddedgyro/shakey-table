/**
 * @file    ina_testing.cpp
 * @author  Adam J. Englebright
 * @date    04.03.2024
 * @brief   This file constains a program for testing I2C comms with the INA260.
 *
 * Copyright 2024 Adam J. Englebright <adamenglebright@rocketmail.com>
 *
 */

#include <cmath>
#include <limits>
#include <thread>
#include <chrono>
#include <iostream>
#include "../lib/pid/pid.h"
#include "../lib/mpu6050/mpu6050.h"
#include "../lib/i2c_interface/smbus_i2c_if.h"
#include "../lib/ina260/ina260.h"
#include "../lib/MotorDriver/MotorDriver.h"


/**
 * @brief Implementation of the INA260Interface for feedback of current (torque)
 * and simply printing the value.
 */
class INA260_Feedback : public INA260_Driver::INA260Interface
{
public:
  /**
   * @brief INA260 callback implementation, passing the measured current (torque) to the provided PID controller object.
   * @param sample Current measured by the INA260 passed to the callback.
   */
  virtual void hasSample(INA260_Driver::INA260Sample& sample) override {
    std::cout << "Current: " << sample.current << " A\n\n";
  }
};


int main() {
  // Setup some settings in variables.
  // INA260 settings:
  INA260_Driver::Alert_Conf INA_AlertMode = INA260_Driver::Alert_Conf::CNVR;
  INA260_Driver::Conv_Time INA_VoltConvTime = INA260_Driver::Conv_Time::TU140;
  INA260_Driver::Conv_Time INA_CurrConvTime = INA260_Driver::Conv_Time::TU8224;
  INA260_Driver::Ave_Mode INA_AveragingMode = INA260_Driver::Ave_Mode::AV1;
  INA260_Driver::Op_Mode INA_OperatingMode = INA260_Driver::Op_Mode::CURCONT;

  // I2C device files and addresses for MPU and INA:
  std::string INA_i2cFile = "/dev/i2c-0";
  uint8_t INA_Address = INA260_ADDRESS;

  // Initialise INA260 object with callback for printing data, and I2C callback for communication.
  INA260_Feedback INA260Callback;
  SMBUS_I2C_IF INA260_I2C_Callback;
  INA260_I2C_Callback.Init_I2C(INA_Address, INA_i2cFile);
  INA260_Driver::INA260 INA260(&INA260_I2C_Callback, &INA260Callback);

  // Setup settings on MPU over i2c.
  INA260.InitializeSensor(INA_AlertMode, INA_VoltConvTime, INA_CurrConvTime, INA_AveragingMode, INA_OperatingMode);

  // Start data aquisition and processing from the MPU.
  INA260.begin();

  // Sleep this thread forever.
  while (true)
    std::this_thread::sleep_for(std::chrono::seconds(86400)); // Check once a day.
}
