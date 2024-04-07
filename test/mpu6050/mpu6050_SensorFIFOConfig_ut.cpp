#include <iostream>
#include <cassert>
#include "MPU6050.h"

// Test case for SetSensor_FIFO_Config method
void testSetSensor_FIFO_Config(const MPU6050& mpu) {
    //MPU6050 mpu6050;

    // Call the method
    i2c_status_t result = mpu.SetSensor_FIFO_Config(250);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetSensor_FIFO_Config test failed!");
}
}

// Test case for GetSensor_FIFO_Config method
void testGetSensor_FIFO_Config(const MPU6050& mpu) {
    //MPU6050 mpu6050;

    // Call the method
    i2c_status_t error;
    int16_t fifoconfigval = mpu.GetSensor_FIFO_Config(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || fifoconfigval != 250) {
        throw std::runtime_error("GetSensor_FIFO_Config test failed!");
    }
}

int main() {

    I2C_Interface i2cInterface();
    MPU6050Interface mpu6050Interface();

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

    try{
    //Execute test case
        testSetSensor_FIFO_Config(mpu6050);
        testGetSensor_FIFO_Config(mpu6050);
    std::cout << "All Sensor_FIFO_Config tests passed!" << std::endl;
    }
    catch (const std::exception& e) {
        //Capturing test case failure
        std::cerr << "Sensor_FIFO_Config test failed: " << e.what() << std::endl;
        return 1; // Returning non-zero exit code to indicate test failure
    }
    return 0;
}
