#include <iostream>
#include <cassert>
#include "MPU6050.h"

// Test case for SetAccel_X_Offset method
void testSetAccelXOffset() {
    MPU6050 mpu6050;

    // Call the method
    i2c_status_t result = mpu6050.SetAccel_X_Offset(100);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetAccelXOffset test failed!");
}
}

// Test case for GetAccel_X_Offset method
void testGetAccelXOffset() {
    MPU6050 mpu6050;

    // Call the method
    i2c_status_t error;
    int16_t offset = mpu6050.GetAccel_X_Offset(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || offset != 100) {
        throw std::runtime_error("GetAccelXOffset test failed!");
    }
}

int main() {

    I2C_Interface i2cInterface();
    MPU6050 mpu6050(&i2cInterface);

    // Initialize the sensor with desired parameters
    i2c_status_t initStatus = mpu6050.InitializeSensor(
        Gyro_FS_t::FULL_SCALE_250,
        Accel_FS_t::FULL_SCALE_2G,
        DLPF_t::BW_260Hz,
        0, // Sample rate divider
        0, // Interrupt configuration
        0  // Interrupt enable
    );

    if (initStatus != I2C_STATUS_SUCCESS) {
        //Capturing initialization failure
        throw std::runtime_error(" Initialization failure!");
        return 1;
    }
    try{
    //Execute test case
        testSetAccelXOffset();
        testGetAccelXOffset();
    std::cout << "All tests passed!" << std::endl;
    }
    catch (const std::exception& e) {
        //Capturing test case failure
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1; // Returning non-zero exit code to indicate test failure
    }
    return 0;
}
