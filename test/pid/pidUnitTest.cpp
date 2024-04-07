/////PID Unit Testing

#include <iostream>
#include <stdexcept>
using namespace std;

#include "pid.h"
class PID_Test : public PID_Interface{
///PID Test class inherited from the base PID_Interface class
    private: 

    double _expected_output;
    double _maxOutput;
    double _minOutput;

    public:
    ///PID TEST class constructor
    PID_Test(double expectedOutput, double maxOutput, double minOutput) : _expected_output(expectedOutput), _maxOutput(maxOutput), _minOutput(minOutput){}
    virtual void hasOutput(double pidOutput) override {
        if ( pidOutput<_minOutput || pidOutput>_maxOutput){
            throw std::runtime_error("Testcase failed!, Outputvalue is out of bound");
        }
        else if (pidOutput != _expected_output){
            throw std::runtime_error("Testcase failed!, Outputvalue doesn't match with the expected value");
        }
        else{
                std::cout<<"Output value matched!"<< std::endl;
        }
    }
};

int main(){
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

