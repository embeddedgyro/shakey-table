#include <iostream>
#include <cassert>
#include "MPU6050.h"

// Test case for SetGyro_SampleRateDivider method
void testSetGyro_SampleRateDivider(const MPU6050& mpu) {
    //MPU6050 mpu6050;

    // Call the method
    i2c_status_t result = mpu.SetGyro_SampleRateDivider(200);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetGyro_SampleRateDivider test failed!");
}
}

// Test case for GetGyro_SampleRateDivider method
void testGetGyro_SampleRateDivider(const MPU6050& mpu) {
    //MPU6050 mpu6050;

    // Call the method
    i2c_status_t error;
    int16_t samplerate = mpu.GetGyro_SampleRateDivider(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || samplerate != 200) {
        throw std::runtime_error("GetGyro_SampleRateDivider test failed!");
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
        testSetGyro_SampleRateDivider(mpu6050);
        testGetGyro_SampleRateDivider(mpu6050);
    std::cout << "All Gyro_SampleRateDivider tests passed!" << std::endl;
    }
    catch (const std::exception& e) {
        //Capturing test case failure
        std::cerr << "Gyro_SampleRateDivider tests failed: " << e.what() << std::endl;
        return 1; // Returning non-zero exit code to indicate test failure
    }
    return 0;
}
