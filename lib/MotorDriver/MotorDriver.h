/**
 * @file    MotorDriver.h
 * @author  Gennady Magin
 * @date    19.03.2024
 * @brief   This file constains the Motor Driver control implementation.
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
#include <gpiodcxx/line-request.hpp>
#include <fstream>
#include <iostream>

/**
  * The main MotorDriver class
  * Sets and controlls the DIR and PWM pins of the MotorDriver
  * Sets the direction and power delivery to the motor
	*/
class MotorDriver
{
  public:
  /**
  * Constructor function for the MotorDriver class
  * Creates a directory for PWM, opens files for PWM control and enables it
  * Sets DIR pin
	* @param _pin_DIR Pi GPIO pin for direction control
	*/
  MotorDriver(uint8_t pin_DIR);


  /**
  * Distructor function for the MotorDriver class
  * Disables PWM and closes files for controlling it
	*/
  ~MotorDriver();
    
  /** 
  * Function to set the duty cycle and direction of the motor driver
  * @param DutyCycle index value between -1 and 1 for setting direction and power delivery to the motor
  */
  void setDutyCycle(double DutyCycle);

  /**
   * Function to change duty cycle by a delta.
   * @param DCdelta amount to change the duty cycle by
   */
  void setDutyCycleDelta(double DCdelta);
    
  protected:
    gpiod::line::offset _pin_DIR = 0; // Declare a DIR output pin
    std::basic_ofstream<char> PWM2_Directory; // Declare a pwm directory file stream object
    std::basic_ofstream<char> PeriodOutputFile; // Declare an output file stream object
    std::basic_ofstream<char> DutyCycleOutputFile; // Declare a duty cycle file stream object
    std::basic_ofstream<char> EnableOutputFile; // Declare an enable file stream object
    gpiod::line_request request_DIR; // Declare a variable for DIR pin control
    const std::filesystem::path chip_path = "/dev/gpiochip2"; // Declare a chip path for manipulating pins
    uint32_t period_PWM = 50000; // Declare period of PWM in nanoseconds
    bool prev_DIR = 0; // Declare a varuable for checking previous motor direction

  /**
   * @brief Current duty cycle
   */
  double currDC = 0;

};
#endif
