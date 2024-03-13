#ifndef CYTRON_MOTOR_DRIVER_H
#define CYTRON_MOTOR_DRIVER_H

class cytronMD{
    public:
        cytronMD(int pwm_pin, int dir_pin_1, int dir_pin_2);    //Constructor function with one PWM Pin and two direction control Pins
        void setSpeed(int speed);                               //Function to set the speed of the motor (Duty cycle manipulation)
    protected:
        int pwm_pin;                                            //Variable declarations as PINs
        int dir_pin_1;
        int dir_pin_2;
};

#endif