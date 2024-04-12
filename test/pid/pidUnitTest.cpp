/**
 * @file    pidUnitTest.cpp
 * @author  Jambulingam Kothandapani
 * @date    30.03.2024
 * @brief   This file constains the unit testing program that does offline validation of PID controller
 * Copyright 2024 Jambulingam Kothandapani <jagandhanasekar@gmail.com>
 *
 */

#include <iostream>
#include <stdexcept>
using namespace std;

#include "pid.h"
class PID_Test : public PID_Interface{
    /**
	 * @brief Class inherited from PID_Interface, overriding the hasOutput function from the parent class
     * and validates the expected and computed pidoutput.
	 * @param _expected_output expected output set manually
     * @param _maxOutput maximum output bound of the value expected
     * @param _minOutput minimum output bound of the value expected 
	 */
    private: 

    double _expected_output;
    double _maxOutput;
    double _minOutput;

    public:
    ///PID Unittest class constructor
    PID_Test(double expectedOutput, double maxOutput, double minOutput) : _expected_output(expectedOutput), _maxOutput(maxOutput), _minOutput(minOutput){}
    
    virtual void hasOutput(double pidOutput) override {
        if ( pidOutput<_minOutput || pidOutput>_maxOutput){
            throw std::runtime_error("PID_Testcase failed!, Outputvalue is out of bound");
        }
        else if (pidOutput != _expected_output){
            throw std::runtime_error("PID_Testcase failed!, Outputvalue doesn't match with the expected value");
        }
        else{
                std::cout<<"Output value matched!"<< std::endl;
        }
    }
};

int main(){
    /**
	 * @brief Class constructor for setting PID attributes such as the PID constants,
	 * and setting the callback interface that will be called when a new PID output is calculated.
	 * @param testObj Instance of the class PID_Test
     * @param pidController Instance of the class PID
	 * @param setpoint Setpoint value
	 * @param dt Sample period
	 * @param max Maximum possible PID output value
	 * @param min Minimum possible PID output value
	 * @param Kp Proportional gain
	 * @param Kd Derivative gain
	 * @param Ki Integral gain
     * @param processVariable Feedback variable defined with random value
	 * @retval None
	 */
    double setpoint = 50.0;
    double dt = 0.1;
    double max = 100.0;
    double min = 0.0;
    double Kp = 0.1;
    double Kd = 0.01;
    double Ki = 0.05;

    PID_Test testObj(0x1.0666666666666p+0, 0x64p+0, 0x0);

    PID pidController(&testObj, setpoint, dt, max, min, Kp, Kd, Ki);

    double processVariable = 45.0;

    pidController.calculate(processVariable);

    return 0;
}

