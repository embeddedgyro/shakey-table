#include <iostream>
#include <cassert>
#include "MPU6050.h"


int main() {
    // Creating the instances for i2cinterface and mpu6050interface classes
    I2C_Interface i2cInterface();
    MPU6050Interface mpu6050Interface();
    //creating instance for mpu6050 and passing i2cinterface and mpu6050interface instances as references
    MPU6050 mpu6050(&i2cInterface, &mpu6050Interface);

    // Initialize the sensor with desired parameters
    i2c_status_t initStatus = mpu6050.InitializeSensor(
        Gyro_FS_t::FULL_SCALE_250,
        Accel_FS_t::FULL_SCALE_2G,
        DLPF_t::BW_260Hz,
        uint8_t SRdiv = 7,  // Sample Rate = Gyroscope Output Rate / (1 + SRdiv)
	    uint8_t INTconf = Regbits_INT_PIN_CFG::BIT_INT_RD_CLEAR,
	    uint8_t INTenable = Regbits_INT_ENABLE::BIT_DATA_RDY_EN;
    );

    if (initStatus != I2C_STATUS_SUCCESS) {
        //Capturing initialization failure
        throw std::runtime_error(" Initialization failure!");
        return 1;
    }
    return 0;
}
