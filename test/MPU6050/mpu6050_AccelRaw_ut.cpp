#include <iostream>
#include <cassert>
#include "MPU6050.h"


// Test case for GetAccel_X_Raw method
void testGetAccel_X_Raw(const MPU6050& mpu) {

    // Call the method
    i2c_status_t error;
    int16_t accelXVal = mpu.GetAccel_X_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || accelXVal == 0x00) {
        throw std::runtime_error("GetAccel_X_Raw test failed!");
    }
}
// Test case for GetAccel_Y_Raw method
void testGetAccel_Y_Raw(const MPU6050& mpu) {


    // Call the method
    i2c_status_t error;
    int16_t accelYVal = mpu.GetAccel_Y_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || accelYVal == 0x00) {
        throw std::runtime_error("GetAccel_Y_Raw test failed!");
    }
}
// Test case for GetAccel_Z_Raw method
void testGetAccel_Z_Raw(const MPU6050& mpu) {

    // Call the method
    i2c_status_t error;
    int16_t accelZVal = mpu.GetAccel_Z_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || accelZVal == 0x00) {
        throw std::runtime_error("GetAccel_Z_Raw test failed!");
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
        testGetAccel_X_Raw(mpu6050);
        testGetAccel_Y_Raw(mpu6050);
        testGetAccel_Z_Raw(mpu6050);
    std::cout << "All tests for GetAccel_Raw passed!" << std::endl;
    }
    catch (const std::exception& e) {
        //Capturing test case failure
        std::cerr << "GetAccel_Raw Test failed: " << e.what() << std::endl;
        return 1; // Returning non-zero exit code to indicate test failure
    }
    return 0;
}