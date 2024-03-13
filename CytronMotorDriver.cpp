#include CytronMotorDriver.h
#include <wiringPi.h>

CytronMD::CytronMD(int PWM_pin, int DIR_pin_1, int DIR_pin_2){
    //local variables of the class holding the input variable values
    _pwm_pin = PWM_pin;
    _dir_pin_1 = DIR_pin_1;
    _dir_pin_2 = DIR_pin_2;

    //Calling Raspberrypi's internal library
    wiringPiSetup();

    //Setting the modes of the declared PINs 
    pinMode(_pwm_pin, PWM_OUTPUT);
    pinMode(_dir_pin_1, OUTPUT);
    pinMode(_dir_pin_2, OUTPUT);

    //Resetting direction control pins to LOW to ensure the motor doesn't start in default case
    digitalwrite(_dir_pin_1, LOW);
    digitalwrite(_dir_pin_2, LOW);
}

void CytronMD:setSpeed(int speed){

    //Setting the speed value within the limits

    if(speed>255){
        speed = 255;
    }
    elseif(speed<-255){
        speed = -255;
    }
    // Set direction
    if(speed>=0){
        digitalwrite(_dir_pin_1, HIGH);  //forward direction rotation pin settings
        digitalwrite(_dir_pin_2, LOW);
    }   
    else {
        digitalwrite(_dir_pin_1, LOW);   //reverse direction rotation pin settings
        digitalwrite(_dir_pin_2, HIGH);
        speed = -speed; ///setting speed value to positive
    }
    pwmwrite(_pwm_pin, speed);

}