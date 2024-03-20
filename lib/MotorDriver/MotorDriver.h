/**
 * @file    MotorDriver.h
 * @author  Gennady Magin
 * @author  Gennady Magin
 * @date    19.03.2024
 * @brief   This file constains the Cytron Motor Driver control implementation.
 *
 * Copyright 2024 Gennady Magin <magingena2001@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <gpiod.hpp>
#include <fstream>
#include <iostream>



class MotorDriver
{
  public:
  /**
  * Constructor function for the class MotorDriver
	* @param _pin_PWM Pi GPIO pin for PWM control
	* @param _value Pi GPIO pin for direction control
	*/
  MotorDriver(uint8_t pin_PWM, uint8_t pin_DIR);
    
  /** 
  * Function to set the duty cycle and direction of motor driver
  */
  void setDutyCycle(float DutyCycle);
    
  protected:
  	uint8_t _pin_PWM;
    uint8_t _pin_DIR;
    std::ofstream outputFile; // Declare an output file stream object
    uint32_t period_PWM = 66666 // Period of PWM in nanoseconds
};

#endif
