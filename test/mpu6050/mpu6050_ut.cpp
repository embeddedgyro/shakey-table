/**
 * @file    mpu6050_Wakeup_ut.cpp
 * @author  Jambulingam Kothandapani
 * @date    02.04.2024
 * @brief   This file constains the unit testing program that does online testing of i2c communcation of sensor wakeup between Pi and the MPU sensor 
 * Copyright 2024 Jambulingam  <jagandhanasekar@gmail.com>
 *
 */


#include <iostream>
#include <cassert>
#include <cmath>
#include <limits>
#include <thread>
#include <chrono>
#include <iostream>
#include "../../lib/pid/pid.h"
#include "../../lib/mpu6050/mpu6050.h"
#include "../../lib/i2c_interface/smbus_i2c_if.h"
#include "../../lib/ina260/ina260.h"
#include "../../lib/MotorDriver/MotorDriver.h"

// Test case for SetAccel_X_Offset method
void testSetAccelXOffset(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for SetAccelXOffset is getting executed" << std::endl;
    // Call the method
    i2c_status_t result = mpu->SetAccel_X_Offset(100);
    std::cout << result << std::endl;

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetAccelXOffset test failed!");
}
}

// Test case for GetAccel_X_Offset method
void testGetAccelXOffset(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for GetAccelXOffset is getting executed" << std::endl;
    // Call the method
    i2c_status_t error;
    int16_t offset = mpu->GetAccel_X_Offset(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || offset != 100) {
        throw std::runtime_error("GetAccelXOffset test failed!");
    }
}

void testGetAccel_X_Raw(MPU6050_Driver::MPU6050* mpu) {

    // Call the method
    std::cout << "Test function for testGetAccel_X_Raw is getting executed" << std::endl;
    i2c_status_t error;
    int16_t accelXVal = mpu->GetAccel_X_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || accelXVal == 0x00) {
        throw std::runtime_error("GetAccel_X_Raw test failed!");
    }
}
// Test case for GetAccel_Y_Raw method
void testGetAccel_Y_Raw(MPU6050_Driver::MPU6050* mpu) {


    // Call the method
    std::cout << "Test function for testGetAccel_Y_Raw is getting executed" << std::endl;
    i2c_status_t error;
    int16_t accelYVal = mpu->GetAccel_Y_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || accelYVal == 0x00) {
        throw std::runtime_error("GetAccel_Y_Raw test failed!");
    }
}
// Test case for GetAccel_Z_Raw method
void testGetAccel_Z_Raw(MPU6050_Driver::MPU6050* mpu) {

    // Call the method
    std::cout << "Test function for testGetAccel_Z_Raw is getting executed" << std::endl;
    i2c_status_t error;
    int16_t accelZVal = mpu->GetAccel_Z_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || accelZVal == 0x00) {
        throw std::runtime_error("GetAccel_Z_Raw test failed!");
    }
}
void testSetGyro_X_Offset(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;

    // Call the method
    std::cout << "Test function for testSetGyro_X_Offset is getting executed" << std::endl;
    i2c_status_t result = mpu->SetGyro_X_Offset(100);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetGyro_X_Offset test failed!");
}
}

// Test case for GetGyro_X_Offset method
void testGetGyro_X_Offset(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;

    // Call the method
    std::cout << "Test function for testGetGyro_X_Offset is getting executed" << std::endl;
    i2c_status_t error;
    int16_t offset = mpu->GetGyro_X_Offset(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || offset != 100) {
        throw std::runtime_error("GetGyro_X_Offset test failed!");
    }
}

void testGetGyro_X_Raw(MPU6050_Driver::MPU6050* mpu) {

    // Call the method
    std::cout << "Test function for testGetGyro_X_Raw is getting executed" << std::endl;
    i2c_status_t error;
    int16_t gyroXVal = mpu->GetGyro_X_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || gyroXVal == 0x00) {
        throw std::runtime_error("GetGyro_X_Raw test failed!");
    }
}
// Test case for GetGyro_X_Raw method
void testGetGyro_Y_Raw(MPU6050_Driver::MPU6050* mpu) {


    // Call the method
    std::cout << "Test function for testGetGyro_Y_Raw is getting executed" << std::endl;
    i2c_status_t error;
    int16_t gyroYVal = mpu->GetGyro_Y_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || gyroYVal == 0x00) {
        throw std::runtime_error("GetGyro_Y_Raw test failed!");
    }
}
// Test case for GetGyro_Z_Raw method
void testGetGyro_Z_Raw(MPU6050_Driver::MPU6050* mpu) {

    // Call the method
    std::cout << "Test function for testGetGyro_Z_Raw is getting executed" << std::endl;
    i2c_status_t error;
    int16_t gyroZVal = mpu->GetGyro_Z_Raw(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || gyroZVal == 0x00) {
        throw std::runtime_error("GetGyro_Z_Raw test failed!");
    }
}
// Test case for SetGyro_SampleRateDivider method
void testSetGyro_SampleRateDivider(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testSetGyro_SampleRateDivider is getting executed" << std::endl;
    // Call the method
    i2c_status_t result = mpu->SetGyro_SampleRateDivider(200);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetGyro_SampleRateDivider test failed!");
}
}

// Test case for GetGyro_SampleRateDivider method
void testGetGyro_SampleRateDivider(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testGetGyro_SampleRateDivider is getting executed" << std::endl;
    // Call the method
    i2c_status_t error;
    int16_t samplerate = mpu->GetGyro_SampleRateDivider(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || samplerate != 200) {
        throw std::runtime_error("GetGyro_SampleRateDivider test failed!");
    }
}

// Test case for SetSensor_FIFO_Config method
void testSetSensor_FIFO_Config(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testSetSensor_FIFO_Config is getting executed" << std::endl;
    // Call the method
    i2c_status_t result = mpu->SetSensor_FIFO_Config(250);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetSensor_FIFO_Config test failed!");
}
}

// Test case for GetSensor_FIFO_Config method
void testGetSensor_FIFO_Config(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testGetSensor_FIFO_Config is getting executed" << std::endl;
    // Call the method
    i2c_status_t error;
    int16_t fifoconfigval = mpu->GetSensor_FIFO_Config(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || fifoconfigval != 250) {
        throw std::runtime_error("GetSensor_FIFO_Config test failed!");
    }
}

void testSetSensor_FIFO_Enable(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testSetSensor_FIFO_Enable is getting executed" << std::endl;
    // Call the method
    i2c_status_t result = mpu->SetSensor_FIFO_Enable(true);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetSensor_FIFO_Enable test failed!");
}
}

// Test case for GetSensor_FIFO_Enable method
void testGetSensor_FIFO_Enable(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testGetSensor_FIFO_Enable is getting executed" << std::endl;
    // Call the method
    i2c_status_t error;
    bool state = mpu->GetSensor_FIFO_Enable(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || state != true) {
        throw std::runtime_error("GetSensor_FIFO_Enable test failed!");
    }
}

void testSetSensor_InterruptPinConfig(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testSetSensor_InterruptPinConfig is getting executed" << std::endl;
    // Call the method
    i2c_status_t result = mpu->SetSensor_InterruptPinConfig(150);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetSensor_InterruptPinConfig test failed!");
}
}
// Test case for GetSensor_InterruptPinConfig method
void testGetSensor_InterruptPinConfig(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for testGetSensor_InterruptPinConfig is getting executed" << std::endl;
    // Call the method
    i2c_status_t error;
    int16_t intPinConfig = mpu->GetSensor_InterruptPinConfig(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || intPinConfig != 150) {
        throw std::runtime_error("GetSensor_InterruptPinConfig test failed!");
    }
}

/**
 * @brief Implementation of the MPU6050Interface. This is where the magic
 * happens for calculating the cup holder's angular position based on the IMU
 * measurements. The caclulated angular position is then sent as input to the
 * outer PID controller as the PV.
 */
class MPU6050_Feedback : public MPU6050_Driver::MPU6050Interface
{
public:
  /**
   * @brief Constructor taking and assigning a PID controller object reference.
   * @param _pidController The PID controller object.
   */
  MPU6050_Feedback(float _radius, float _samplePeriod)
    :  radius(_radius), samplePeriod(_samplePeriod) {}

  /**
   * @brief MPU6050 callback implementation. Takes the sample data and caclulates the angular position of the cup holder,
   * which is then passed as input to the outer PID controller as the PV.
   */
  virtual void hasSample(MPU6050_Driver::MPU6050Sample& sample) override {
    /*
     * The maths here assumes:
     * 1. The MPU is mounted with the chip, SMD components, axis info etc. visible (towards the back) and the text the right way round. README should have a pic.
     * 2. Looking from the back of the cup holder, onto the face of the MPU, a positive angular displacement occurs clockwise, and negative anti-clockwise.
     * 3. Angular displacement is zero when the cup holder is upright.
     */

    // Adjust y accel component for centripetal acceleration caused by angular velocity around z axis
    // Watch units. Sample linear acceleration is in g. Convert to m/s^2.
    // Watch units. Sample angular velocity is in deg/s. Convert to rad/s.
    float ayUnitsCorrected = sample.ay * 9.80665;
    float gzUnitsCorrected = sample.gz * 3.14159265358979323846 / 180.0;
    float ayGrav = ayUnitsCorrected + gzUnitsCorrected * gzUnitsCorrected * radius;

    // Adjust x accel component for tangential acceleration caused by angular acceleration around z axis
    // Watch units. Sample linear acceleration is in g. Convert to m/s^2.
    // Watch units. Sample angular velocity is in deg/s. Convert to rad/s.
    float axUnitsCorrected = sample.ax * 9.80665;
    float axGrav = axUnitsCorrected + ((gzUnitsCorrected - gzPrev) / samplePeriod) * radius;
    gzPrev = gzUnitsCorrected;

    // Caculate magnitude of gravity in xy plane.
    float gravMag = std::sqrt(axGrav * axGrav + ayGrav * ayGrav);

    // Calculate angular displacement in rad from upright.
    float angularPos = std::acos(ayGrav / gravMag);

    // Ensure sign of angular position is correct.
    if (axGrav > 0)
      angularPos = -angularPos;

    float angularPosDeg = angularPos * 180.0 / 3.14159265358979323846;
    /*
    // Print data.
    std::cout << "Accel X = " << sample.ax << " g, Gyro X = " << sample.gx << " deg/s" << std::endl;
    std::cout << "Accel Y = " << sample.ay << " g, Gyro Y = " << sample.gy << " deg/s" << std::endl;
    std::cout << "Accel Z = " << sample.az << " g, Gyro Z = " << sample.gz << " deg/s" << std::endl;
    std::cout << "Calculated angular postion = " << angularPos << " rad" << std::endl << std::endl;
    std::cout << "Calculated angular postion = " << angularPosDeg << " deg" << std::endl << std::endl;
    */
  }

private:

  /**
   * @brief Radius from axis of rotation to MPU in meters.
   */
  float radius;

  /**
   * @brief Sample period for tangential acceleration calculation in seconds.
   */
  float samplePeriod;

  /**
   * @brief Previous angular velocity around z axis for tangential acceleration calculation (stored in rad/s).
   */
  float gzPrev = 0;
};

int main() {

    // Setup some settings in variables.
    // MPU6050 settings:
    MPU6050_Driver::Gyro_FS_t MPU_GyroScale = MPU6050_Driver::Gyro_FS_t::FS_250_DPS;
    MPU6050_Driver::Accel_FS_t MPU_AccelScale = MPU6050_Driver::Accel_FS_t::FS_2G;
    MPU6050_Driver::DLPF_t MPU_DLPFconf = MPU6050_Driver::DLPF_t::BW_184Hz;
    uint8_t MPU_SRdiv = 25;
    uint8_t MPU_INTconf = MPU6050_Driver::Regbits_INT_PIN_CFG::BIT_INT_RD_CLEAR;
    uint8_t MPU_INTenable = MPU6050_Driver::Regbits_INT_ENABLE::BIT_DATA_RDY_EN;

    // For an explanation of bellow, see https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf, page 12.
    float MPU_SamplePeriod;
    if (MPU_DLPFconf == MPU6050_Driver::DLPF_t::BW_260Hz || MPU_DLPFconf == MPU6050_Driver::DLPF_t::RESERVED)
        MPU_SamplePeriod = (1.0 + (float)MPU_SRdiv) / 8000.0;
    else
        MPU_SamplePeriod = (1.0 + (float)MPU_SRdiv) / 1000.0;

    // Radius from axis of ratation to MPU chip (need to actually measure this):
    float radius = 0.15;
    // I2C device files and addresses for MPU and INA:
    std::string MPU_i2cFile = "/dev/i2c-1";
    uint8_t MPU_Address = MPU6050_ADDRESS;
    
    // Initialise MPU6050 object with callback using the outer PID controller, and I2C callback for communication.
    std::cout << "MPU6050 instance creation" << std::endl;
    MPU6050_Feedback MPU6050Callback(radius, MPU_SamplePeriod);
    SMBUS_I2C_IF MPU6050_I2C_Callback;
    MPU6050_I2C_Callback.Init_I2C(MPU_Address, MPU_i2cFile);
    MPU6050_Driver::MPU6050 MPU6050(&MPU6050_I2C_Callback, &MPU6050Callback, 17);
    //Initialize the sensor with desired parameters
    i2c_status_t initStatus = MPU6050.InitializeSensor(MPU_GyroScale, MPU_AccelScale, MPU_DLPFconf, MPU_SRdiv, MPU_INTconf, MPU_INTenable);
    //std::cout << initStatus << std::endl;
    if (initStatus != I2C_STATUS_SUCCESS) {
        //Capturing initialization failure
        //std::cout <<"Throwing"<< std::endl;
        throw std::runtime_error("Initialization failure!");
        return 1;
    }
    std::cout<<"Initialization successfull!"<< std::endl;
    //Execute test case
        std::cout<<"Test cases getting executed" << std::endl;
        testSetAccelXOffset(&MPU6050);
        testGetAccelXOffset(&MPU6050);
        testGetAccel_Z_Raw(&MPU6050);
        testSetGyro_X_Offset(&MPU6050);
        testGetGyro_X_Offset(&MPU6050);
        testGetGyro_X_Raw(&MPU6050);
        testGetGyro_Y_Raw(&MPU6050);
        testGetGyro_Z_Raw(&MPU6050);
        testSetGyro_SampleRateDivider(&MPU6050);
        testGetGyro_SampleRateDivider(&MPU6050);
        testSetSensor_FIFO_Config(&MPU6050);
        testGetSensor_FIFO_Config(&MPU6050);
        testSetSensor_FIFO_Enable(&MPU6050);
        testGetSensor_FIFO_Enable(&MPU6050);
        testSetSensor_InterruptPinConfig(&MPU6050);
        testGetSensor_InterruptPinConfig(&MPU6050);
        std::cout << "All tests passed!" << std::endl;
        MPU6050.begin();
    return 0;
}

