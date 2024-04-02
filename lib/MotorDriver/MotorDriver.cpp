/**
 * @file    MotorDriver.cpp
 * @author  Jambulingam Kothandapani
 * @date    19.03.2024
 * @brief   This file constains the Motor Driver control implementation.
 *
 * Copyright 2024 Jambulingam Kothandapani <jagandhanasekar@gmail.com>
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

#include <gpiod.hpp>
#include <gpiodcxx/line-request.hpp>
#include <iostream>
#include <fstream>
#include "MotorDriver.h"

MotorDriver::MotorDriver(uint8_t pin_DIR)
:
request_DIR(::gpiod::chip(chip_path)
			       .prepare_request()
			       .set_consumer("set-line-direction")
			       .add_line_settings(_pin_DIR, ::gpiod::line_settings()
                         .set_direction(::gpiod::line::direction::OUTPUT))
			       .do_request())

{
      _pin_DIR = pin_DIR;

      // Presetting DIR pin to output
      request_DIR = ::gpiod::chip(chip_path)
			       .prepare_request()
			       .set_consumer("set-line-direction")
			       .add_line_settings(_pin_DIR, ::gpiod::line_settings()
                         .set_direction(::gpiod::line::direction::OUTPUT))
			       .do_request();


      // Presetting DIR pin to low
      request_DIR.set_value(_pin_DIR, ::gpiod::line::value::INACTIVE);

      // Create pwm2 directory and open files for controlling PWM
      // Initialise PWM by enabling it and presetting duty cycle to zero
      PWM2_Directory.open("/sys/class/pwm/pwmchip2/export", std::ios::out | std::ios::trunc);
      if (PWM2_Directory.is_open()) 
      {
            PWM2_Directory << 2 << std::endl;
      }
      else 
      {
            std::cout << "Failed to create pwm2 directory." << std::endl; // Display an error message if file opening failed
            throw std::invalid_argument( "Failed to create pwm2 directory." );
      }

      PeriodOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/period", std::ios::out | std::ios::trunc);
      if (PeriodOutputFile.is_open()) 
      {
            PeriodOutputFile << period_PWM << std::endl;
      }
      else 
      {
            std::cout << "Failed to open period file." << std::endl; // Display an error message if file opening failed
            throw std::invalid_argument( "Failed to open period file." );
      }

      DutyCycleOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/duty_cycle", std::ios::out | std::ios::trunc);
      if ( DutyCycleOutputFile.is_open())
      {
            DutyCycleOutputFile << 0 << std::endl;
      }
      else 
      {
            std::cout << "Failed to open duty_cycle file." << std::endl; // Display an error message if file opening failed
            throw std::invalid_argument( "Failed to open duty_cycle file." );
      }

      EnableOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/enable", std::ios::out | std::ios::trunc);
      if (EnableOutputFile.is_open()){
            EnableOutputFile << 1 << std::endl;
      }
      else 
      {
            std::cout << "Failed to open enable file." << std::endl; // Display an error message if file opening failed
            throw std::invalid_argument( "Failed to open enable file." );
      }

}

void MotorDriver::setDutyCycle(double DutyCycle)
{
      // Make sure the DutyCycle is within the limit.
      if (DutyCycle > 1.0)
      {
            DutyCycle = 1.0;
      }
      else if (DutyCycle < -1.0)
      {
            DutyCycle = -1.0;
      }

      // Record current duty cycle
      currDC = DutyCycle;

      // Convert duty cycle into appropriate form
      uint32_t Duty_nanosec = DutyCycle*period_PWM;

      // Set duty cycle and direction.
      if (DutyCycle >= 0 && DutyCycle <= 1)
      {
            //forward motion
            if (prev_DIR == 1)
            {
                  request_DIR.set_value(_pin_DIR, ::gpiod::line::value::ACTIVE);
                  prev_DIR = 0;
            }
      
            if (DutyCycleOutputFile.is_open())
            {
                  DutyCycleOutputFile <<  Duty_nanosec << std::endl;
            }
            else 
            {
                  std::cout << "Failed to open duty_cycle file." << std::endl; // Display an error message if file opening failed
                  throw std::invalid_argument( "Failed to open duty_cycle file." );
            }
      }
      else if (DutyCycle < 0 && DutyCycle >= -1)
      {
            //backwards motion
            if (prev_DIR == 0)
            {
                  request_DIR.set_value(_pin_DIR, ::gpiod::line::value::ACTIVE);
                  prev_DIR = 1;
            }
            
            if (DutyCycleOutputFile.is_open())
            {
                  DutyCycleOutputFile << Duty_nanosec << std::endl;
            }
            else 
            {
                  std::cout << "Failed to open duty_cycle file." << std::endl; // Display an error message if file opening failed
                  throw std::invalid_argument( "Failed to open duty_cycle file." );
            }
      }
      else
      {
            std::cout << "Duty Cycle out of limit." << std::endl; // Display an error message if file Duty Cycle is out of limits
            throw std::invalid_argument( "Duty Cycle out of limit." ); 
      }
}


void MotorDriver::setDutyCycleDelta(double DCdelta) { setDutyCycle(currDC + DCdelta); }

MotorDriver::~MotorDriver()
{
      // Disable PWM output and close all PWM control files
      PeriodOutputFile.close();
      DutyCycleOutputFile.close();
      EnableOutputFile << 0 << std::endl;
      EnableOutputFile.close();
      PWM2_Directory.close();
      if (PeriodOutputFile.is_open())
            {
                  std::cout << "Failed to close period file." << std::endl; // Display an error message if file closing failed
                  throw std::invalid_argument( "Failed to close period file." );
            }
      if (DutyCycleOutputFile.is_open())
            {
                  std::cout << "Failed to close duty_cycle file." << std::endl; // Display an error message if file closing failed
                  throw std::invalid_argument( "Failed to close duty_cycle file." );
            }
      if (EnableOutputFile.is_open())
            {
                  std::cout << "Failed to close enable file." << std::endl; // Display an error message if file closing failed
                  throw std::invalid_argument( "Failed to close enable file." );
            }
      if (PWM2_Directory.is_open())
            {
                  std::cout << "Failed to close pwm2 directory." << std::endl; // Display an error message if file closing failed
                  throw std::invalid_argument( "Failed to close pwm2 directory." );
            }
}
