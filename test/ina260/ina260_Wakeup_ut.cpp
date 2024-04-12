/**
 * @file    ina260_Wakeup_ut.cpp
 * @author  Jambulingam Kothandapani
 * @date    05.04.2024
 * @brief   This file constains the unit testing program that does online testing of i2c communcation of sensor wakeup between Pi and the INA sensor 
 * Copyright 2024 Jambulingam  <jagandhanasekar@gmail.com>
 *
 */

#include <cmath>
#include <fstream>
#include <limits>
#include <thread>
#include <chrono>
#include <iostream>
#include "../../lib/i2c_interface/smbus_i2c_if.h"
#include "../../lib/ina260/ina260.h"


/**
 * @brief Implementation of the INA260Interface for feedback of current (torque)
 * and simply printing the value.
 */
class INA260_Feedback : public INA260_Driver::INA260Interface
{
public:
  INA260_Feedback(std::string file) : log_file(file, std::ios::trunc) {}
  
  /**
   * @brief INA260 callback implementation, passing the measured current (torque) to the provided PID controller object.
   * @param sample Current measured by the INA260 passed to the callback.
   */
  virtual void hasSample(INA260_Driver::INA260Sample& sample) override {
    std::cout << "Current: " << sample.current << " A\n\n";
    log_file << sample.current << std::endl;
  }

private:
  std::ofstream log_file;
};


int main() {
  // Setup some settings in variables.
  // INA260 settings:
  INA260_Driver::Alert_Conf INA_AlertMode = INA260_Driver::Alert_Conf::CNVR;
  INA260_Driver::Conv_Time INA_VoltConvTime = INA260_Driver::Conv_Time::TU140;
  INA260_Driver::Conv_Time INA_CurrConvTime = INA260_Driver::Conv_Time::TU4156;
  INA260_Driver::Ave_Mode INA_AveragingMode = INA260_Driver::Ave_Mode::AV1;
  INA260_Driver::Op_Mode INA_OperatingMode = INA260_Driver::Op_Mode::CURCONT;

  // I2C device files and addresses for MPU and INA:
  std::string INA_i2cFile = "/dev/i2c-1";
  uint8_t INA_Address = INA260_ADDRESS;

  // Initialise INA260 object with callback for printing data, and I2C callback for communication.
  INA260_Feedback INA260Callback("current_data");
  SMBUS_I2C_IF INA260_I2C_Callback;
  INA260_I2C_Callback.Init_I2C(INA_Address, INA_i2cFile);
  INA260_Driver::INA260 INA260(&INA260_I2C_Callback, &INA260Callback, 5);

  // Setup settings on MPU over i2c.
  i2c_status_t initStatus = INA260.InitializeSensor(INA_AlertMode, INA_VoltConvTime, INA_CurrConvTime, INA_AveragingMode, INA_OperatingMode);
    if (initStatus != I2C_STATUS_SUCCESS) {
        //Capturing initialization failure
        throw std::runtime_error(" Initialization failure!");
        return 1;
    }
    std::cout << "Initialization successfull!" << std::endl;
    INA260.begin();
    return 0;

}
