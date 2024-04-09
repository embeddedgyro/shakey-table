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
#include <iostream>
#include <cmath>
#include "MotorDriver.h"

MotorDriver::MotorDriver(const std::filesystem::path chip_path, gpiod::line::offset pin_DIR, uint32_t period_ns)
:
period_PWM(period_ns),
_pin_DIR(pin_DIR),
request_DIR(::gpiod::chip(chip_path)
			       .prepare_request()
			       .set_consumer("set-line-direction")
			       .add_line_settings(_pin_DIR, ::gpiod::line_settings()
                         .set_direction(::gpiod::line::direction::OUTPUT))
			       .do_request())

{
      std::cout << "MotorDriver constructor entered." << std::endl;

      // Presetting DIR pin to low
      request_DIR.set_value(_pin_DIR, ::gpiod::line::value::INACTIVE);

      // Create pwm2 directory and open files for controlling PWM
      // Initialise PWM by enabling it and presetting duty cycle to zero
      PWM2_Export.open("/sys/class/pwm/pwmchip2/export", std::ios::out | std::ios::trunc);
      if (PWM2_Export.is_open()) 
      {
            PWM2_Export << 2 << std::endl;
      }
      else 
      {
            std::cout << "Failed to create pwm2 directory." << std::endl; // Display an error message if file opening failed
            throw std::invalid_argument( "Failed to create pwm2 directory." );
      }

      // Open PWM unexport file to close pwm2 directory when finished.
      PWM2_Unexport.open("/sys/class/pwm/pwmchip2/unexport", std::ios::out | std::ios::trunc);
      if (!PWM2_Unexport.is_open()) 
      {
            std::cout << "Failed to open pwm unexport file." << std::endl; // Display an error message if file opening failed
            throw std::invalid_argument( "Failed to open pwm unexport file." );
      }


      PeriodOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/period", std::ios::out | std::ios::trunc);

      /* For some reason the "period" file cannot be opened immediately after writing "2" to the "export" file.
       * The "period" file does exist immediately after the "export" write, which can be confirmed using
       * std::filesystem::exists(), but is not openable. Maybe the kernel is still accessing them?
       * So, we loop round trying to open the file until it can be opened.
       */
      while (!PeriodOutputFile.is_open())
	    PeriodOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/period", std::ios::out | std::ios::trunc);
      
      if (PeriodOutputFile.is_open()) // Bit redundant now. Maybe add timeout for above loop, so this is still relevant?
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
      uint32_t Duty_nanosec = (uint32_t)(std::abs(DutyCycle) * period_PWM);

      // Set duty cycle and direction.
      if (DutyCycle >= 0 && DutyCycle <= 1)
      {
            //forward motion
            if (prev_DIR == 1)
            {
                  request_DIR.set_value(_pin_DIR, ::gpiod::line::value::ACTIVE);
                  prev_DIR = 0;
		  std::cout << "Changing direction. Dir pin should be HIGH." << std::endl;
            }
      
            if (DutyCycleOutputFile.is_open())
            {
                  DutyCycleOutputFile <<  Duty_nanosec << std::endl;
		  std::cout << "Set duty cycle to " << Duty_nanosec << " in the 'forward' direction." << std::endl;
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
                  request_DIR.set_value(_pin_DIR, ::gpiod::line::value::INACTIVE);
                  prev_DIR = 1;
		  std::cout << "Changing direction. Dir pin should be LOW." << std::endl;
            }
            
            if (DutyCycleOutputFile.is_open())
            {
                  DutyCycleOutputFile << Duty_nanosec << std::endl;
		  std::cout << "Set duty cycle to " << Duty_nanosec << " in the 'backward' direction." << std::endl;
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
      PWM2_Export.close();
      PWM2_Unexport << 2 << std::endl;
      PWM2_Unexport.close();
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
      if (PWM2_Export.is_open())
            {
                  std::cout << "Failed to close pwm2 directory." << std::endl; // Display an error message if file closing failed
                  throw std::invalid_argument( "Failed to close pwm2 directory." );
                  
            }
}
