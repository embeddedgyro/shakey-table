/**
 * @file    Motordriver_DutyCycle_ut.cpp
 * @author  Gennady Magin
 * @date    30.03.2024
 * @brief   This file constains the unit testing program that does offline validation of Motor driver class
 * Copyright 2024 Gennady Magin <magingena2001@gmail.com>
 *
 */

#include <gpiod.hpp>
#include <gpiodcxx/line-request.hpp>
#include <cmath>
#include <fstream>
#include <limits>
#include <thread>
#include <chrono>
#include <iostream>
#include "../../lib/MotorDriver/MotorDriver.h"

int main(){
    
    // Gpiod device file path:
  std::filesystem::path chip_path("/dev/gpiochip4");

  // Motor driver direction GPIO pin:
  gpiod::line::offset MD_DirPin = 23;

  std::cout << "Set up variables." << std::endl;

  // Initialise motor driver object. Assuming line offset 16 for direction pin is correct for now.
  MotorDriver MD20(chip_path, MD_DirPin, 50000);

  std::cout << "Set up motor driver object." << std::endl;

    //Defining an arry that contains set of duty cycle values so that each value can be validated when attempted to set through SetDutyCycle function
    double test_array[] = {1.5, -1.2, 1.0, -1.0, 0.5343, 0};

    for (double i: test_array)  
    {
        std::cout << "setting duty cycle to:" << i << std::endl;
        MD20.setDutyCycle(i);
    }

    std::cout << "Duty cycle Sets are successful!" << std::endl;
    //returning 0 when no exceptions has been encountered
    return 0;
}



