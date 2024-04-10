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

    double test_array[] = {1.5, -1.2, 1.0, -1.0, 0.5343, 0};

    for (double i: test_array)  
    {
        std::cout << "setting duty cycle to:" << i << std::endl;
        MD20.setDutyCycle(i);
    }
    std::cout << "Duty cycle Sets are successful!" << std::endl;
    return 0;
}



