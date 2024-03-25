/**
 * @file    MotorDriver.cpp
 * @author  Jambulingam Kothandapani
 * @author  Jambulingam Kothandapani
 * @date    19.03.2024
 * @brief   This file constains the Cytron Motor Driver control implementation.
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
#include <iostream>
#include <fstream>
#include "MotorDriver.h"

MotorDriver::MotorDriver(uint8_t pin_PWM, uint8_t pin_DIR)
{
      _pin_PWM = pin_PWM;
      _pin_DIR = pin_DIR;

      //::gpiod::chip chip(chip);
      //::gpiod::chip chip("gpiochip0");

      //setting pins for Motor Driver control
      auto request_PWM_out = ::gpiod::chip("/dev/gpiochip2")
			       .prepare_request()
			       .set_consumer("set-line-direction")
			       .add_line_settings(
				       _pin_PWM,
				       ::gpiod::line_settings().set_direction(::gpiod::line::direction::OUTPUT))
			       .do_request();

      auto request_DIR_out = ::gpiod::chip("/dev/gpiochip2")
			       .prepare_request()
			       .set_consumer("set-line-direction")
			       .add_line_settings(
				       _pin_DIR,
				       ::gpiod::line_settings().set_direction(::gpiod::line::direction::OUTPUT))
			       .do_request();


      //presetting pins to low
      auto request_PWM_value = ::gpiod::chip("/dev/gpiochip2")
			       .prepare_request()
			       .set_consumer("set-line-value")
			       .add_line_settings(
				       _pin_PWM,
				       ::gpiod::line_settings().set_output_value(::gpiod::line::value::INACTIVE))
                         .do_request();

      auto request_PWM_value = ::gpiod::chip("/dev/gpiochip2")
			       .prepare_request()
			       .set_consumer("set-line-value")
			       .add_line_settings(
				       _pin_DIR,
				       ::gpiod::line_settings().set_output_value(::gpiod::line::value::INACTIVE))
                         .do_request();
      


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

      PeriodOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/period", std::ios::out | std::ios::trunc);
      if (PeriodOutputFile.is_open()) {
            PeriodOutputFile << period_PWM << std::endl;
            PeriodOutputFile.close();
      }
      else {
      std::cout << "Failed to open the file." << std::endl; // Display an error message if file opening failed
      }
      DutyCycleOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/duty_cycle", std::ios::out | std::ios::trunc);
      EnableOutputFile.open("/sys/class/pwm/pwmchip2/pwm2/enable", std::ios::out | std::ios::trunc);

}

void MotorDriver::setDutyCycle(float DutyCycle)
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

      if (DutyCycle >= 0)
      {
            //forward motion
            auto request_PWM_value = ::gpiod::chip("/dev/gpiochip2")
			       .prepare_request()
			       .set_consumer("set-line-value")
			       .add_line_settings(
				       _pin_DIR,
				       ::gpiod::line_settings().set_output_value(::gpiod::line::value::INACTIVE))
                         .do_request();
      
            if (DutyCycleOutputFile.is_open()){
                  DutyCycleOutputFile << DutyCycle << std::endl;
            }
            else {
            std::cout << "Failed to open the file." << std::endl; // Display an error message if file opening failed
            }

            if (EnableOutputFile.is_open()){
                  EnableOutputFile << 1 << std::endl;
            }
            else {
            std::cout << "Failed to open the file." << std::endl; // Display an error message if file opening failed
            }

            //gpioPWM(_pin_PWM, DutyCycle);
            //gpioWrite(_pin_DIR, 0);
      }
      else
      {
            //backwards motion
            auto request_PWM_value = ::gpiod::chip("/dev/gpiochip2")
			       .prepare_request()
			       .set_consumer("set-line-value")
			       .add_line_settings(
				       _pin_DIR,
				       ::gpiod::line_settings().set_output_value(::gpiod::line::value::ACTIVE))
                         .do_request();
      
            
            if (DutyCycleOutputFile.is_open()){
                  DutyCycleOutputFile << -DutyCycle << std::endl;
            }
            else {
            std::cout << "Failed to open the file." << std::endl; // Display an error message if file opening failed
            }

            if (EnableOutputFile.is_open()){
                  EnableOutputFile << 1 << std::endl;
            }
            else {
            std::cout << "Failed to open the file." << std::endl; // Display an error message if file opening failed
            }

            //gpioPWM(_pin_PWM, -DutyCycle);
            //gpioWrite(_pin_DIR, 1);


            
            //std::this_thread::sleep_for(std::chrono::milliseconds(1));
            
            //std::this_thread::sleep_for(std::chrono::milliseconds(19));
      
      }
}

MotorDriver::~MotorDriver(){
      DutyCycleOutputFile.close();
      EnableOutputFile << 0 << std::endl;
      EnableOutputFile.close();
}