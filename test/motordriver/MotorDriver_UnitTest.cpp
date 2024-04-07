#include <gpiod.hpp>
#include <gpiodcxx/line-request.hpp>
#include <iostream>
#include <fstream>
#include "MotorDriver.h"


int main(){

    uint8_t pin_DIR = 18;

    MotorDriver::MotorDriver(pin_DIR);

    double test_array[] = {1.5, -1.2, 1.0, -1.0, 0.5343, 0};

    for (double i: test_array)  
    {
        MotorDriver::setDutyCycle(i);
    }

    return 0;
}



