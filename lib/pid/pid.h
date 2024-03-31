/**
 * @file    pid.h
 * @author  Bradley J. Snyder
 * @author  Adam J. Englebright
 * @date    13.03.2024
 * @brief   This file constains the PID controller header.
 *
 * Copyright 2019 Bradley J. Snyder <snyder.bradleyj@gmail.com>
 * Copyright 2024 Adam J. Englebright <adamenglebright@rocketmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _PID_H_
#define _PID_H_

/**
 * @brief PID controller callback interface.
 */
class PID_Interface
{
    public:
	/**
	 * @breif Method to be called when a new PID output is generated.
	 * @param pidOutput Output of the PID controller passed to the callback.
	 */
	virtual void hasOutput(double pidOutput) = 0;
};

/**
 * @brief PID controller class.
 */
class PID
{
    public:
	/**
	 * @brief Class constructor for setting PID attributes such as the PID constants,
	 * and setting the callback interface that will be called when a new PID output is calculated.
	 * @param pidInterface PID callback interface pointer
	 * @param setpoint Setpoint value
	 * @param dt Sample period
	 * @param max Maximum possible PID output value
	 * @param min Minimum possible PID output value
	 * @param Kp Proportional gain
	 * @param Kd Derivative gain
	 * @param Ki Integral gain
	 * @retval None
	 */
        PID(PID_Interface* pidInterface, double setpoint, double dt, double max, double min, double Kp, double Kd, double Ki);

	/**
	 * @brief This method will take a process variable, do PID stuff using this process variable and the
	 * object attributes that contain the current setpoint, sample period, PID constants etc., then
	 * pass the PID output to the registered callback.
	 * @param pv Process variable (i.e. the feedback value)
	 * @retval None
	 */
        void calculate(double pv);
	
	/**
	 * @brief Setter to set the PID setpoint (desired plant output)
	 * @param setpoint Setpoint value
	 * @retval None
	 */
	void setSetpoint(double setpoint) { _setpoint = setpoint; }

    private:
	/** Sample period */
        double _dt;

	/** Maximum possible PID output value */
        double _max;

	/** Minimum possible PID output value */
        double _min;

	/** Proportional gain */
        double _Kp;

	/** Derivative gain */
        double _Kd;

	/** Integral gain */
        double _Ki;

	/** Previous error (acts as memory for derivative calculations) */
        double _pre_error;

	/** Accumulated integral for calculating integral component of the PID output */
        double _integral;

	/** Setpoint value */
	double _setpoint;

	/** Pointer to registered PID interface */
	PID_Interface* _PIDcb = nullptr;
};

#endif
