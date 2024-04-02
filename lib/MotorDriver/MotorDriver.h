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
  MotorDriver(const std::filesystem::path chip_path, gpiod::line::offset pin_DIR);



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
    /**
    * @brief DIR output pin
    */
    gpiod::line::offset _pin_DIR;
    /**
    * @brief PWM directory file stream object
    */
    std::basic_ofstream<char> PWM2_Directory;
    /**
    * @brief Output file stream object
    */
    std::basic_ofstream<char> PeriodOutputFile;
    /**
    * @brief Duty Cycle file stream object
    */
    std::basic_ofstream<char> DutyCycleOutputFile;
    /**
    * @brief Enable file stream object
    */
    std::basic_ofstream<char> EnableOutputFile;
    /**
    * @brief Variable for DIR pin control
    */
    gpiod::line_request request_DIR; 
    /**
    * @brief Period of PWM in nanoseconds
    */
    uint32_t period_PWM = 50000;
    /**
    * @brief Previous motor direction
    */
    bool prev_DIR = 0;  
    /**
    * @brief Current duty cycle
    */
    double currDC = 0;
};
#endif
