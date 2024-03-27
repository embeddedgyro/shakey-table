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
      //_pin_PWM = pin_PWM;
      _pin_DIR = pin_DIR;

      //::gpiod::chip chip(chip);
      //::gpiod::chip chip("gpiochip0");

      //setting pins for Motor Driver control
      //auto request_PWM = ::gpiod::chip("/dev/gpiochip2")
	//		       .prepare_request()
	//		       .set_consumer("set-line-direction")
	//		       .add_line_settings(
	//			       _pin_PWM,
	//			       ::gpiod::line_settings().set_direction(::gpiod::line::direction::OUTPUT))
	//		       .do_request();

      request_DIR = ::gpiod::chip(chip_path)
			       .prepare_request()
			       .set_consumer("set-line-direction")
			       .add_line_settings(_pin_DIR, ::gpiod::line_settings()
                         .set_direction(::gpiod::line::direction::OUTPUT))
			       .do_request();


      //presetting DIR pin to low
      request_DIR.set_value(_pin_DIR, ::gpiod::line::value::INACTIVE);

      //auto output_chip = gpiod_chip_open_by_number(2);
      //auto PWM_line = gpiod_chip_get_line(output_chip, _pin_PWM);
      //gpiod_line_request_output(PWM_line, "PWM", GPIOD_LINE_ACTIVE_STATE_HIGH);
      //auto DIR_line = gpiod_chip_get_line(output_chip, _pin_DIR);
      //auto _pin_PWM = chip.get_line(pin_PWM);
      //auto _pin_DIR = chip.get_line(pin_DIR);

      //:gpiod::line_settings().set_output_value(::gpiod::line::value::INACTIVE)
      //::gpiod::line::value set_output_value(::gpiod::line::value::INACTIVE);
      //::gpiod::line::value set_output_value(::gpiod::line::value::INACTIVE);
      //gpiod_line_set_value(PWM_line, 0);
      //gpiod_line_set_value(DIR_line, 0);
      //_pin_PWM.request({"example", gpiod::line_request::DIRECTION_OUTPUT, 0},0);
      //_pin_DIR.request({"example", gpiod::line_request::DIRECTION_OUTPUT, 0},0);
      
      PWM2_Directory.open("/sys/class/pwm/pwmchip2/export", std::ios::out | std::ios::trunc);
      if (PWM2_Directory.is_open()) 
      {
            PWM2_Directory << 2 << std::endl;
      }
      else 
      {
            std::cout << "Failed to create pwm2 directory." << std::endl; // Display an error message if file opening failed
      }

      PeriodOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/period", std::ios::out | std::ios::trunc);
      if (PeriodOutputFile.is_open()) 
      {
            PeriodOutputFile << period_PWM << std::endl;
      }
      else 
      {
            std::cout << "Failed to open period file." << std::endl; // Display an error message if file opening failed
      }

      DutyCycleOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/duty_cycle", std::ios::out | std::ios::trunc);
      if ( DutyCycleOutputFile.is_open())
      {
            DutyCycleOutputFile << 0 << std::endl;
      }
      else 
      {
            std::cout << "Failed to open duty_cycle file." << std::endl; // Display an error message if file opening failed
      }

      EnableOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/enable", std::ios::out | std::ios::trunc);
      if (EnableOutputFile.is_open()){
            EnableOutputFile << 1 << std::endl;
      }
      else {
            std::cout << "Failed to open enable file." << std::endl; // Display an error message if file opening failed
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

      // Set the DutyCycle and direction.

      uint32_t Duty_nanosec = DutyCycle*period_PWM;


      if (DutyCycle >= 0)
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
            }

            //gpioPWM(_pin_PWM, DutyCycle);
            //gpioWrite(_pin_DIR, 0);
      }
      else
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
            }
            
            //gpioPWM(_pin_PWM, -DutyCycle);
            //gpioWrite(_pin_DIR, 1);


            
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
            //std::this_thread::sleep_for(std::chrono::milliseconds(19));
      
      }
}

MotorDriver::~MotorDriver(){
      PeriodOutputFile.close();
      DutyCycleOutputFile.close();
      EnableOutputFile << 0 << std::endl;
      EnableOutputFile.close();
      PWM2_Directory.close();
}