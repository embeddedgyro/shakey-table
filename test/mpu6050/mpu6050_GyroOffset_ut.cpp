/**
 * @file    mpu6050_GyroOffset_ut.cpp
 * @author  Jambulingam Kothandapani
 * @date    02.04.2024
 * @brief   This file constains the unit testing program that does online testing of i2c communcation of setting and getting Gyro offset value between Pi and the MPU sensor 
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

// Test case for SetGyro_X_Offset method
/**
 * @brief Gets the instance of the class mpu and calls the function SetGyro_X_Offset.
 * Throws runtime error if the Configuration is not successful.
 * @param result stores the result of the function call.
 * @return None
 */
void testSetGyro_X_Offset(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for SetGyro_X_Offset is getting executed" << std::endl;
    // Call the method
    i2c_status_t result = mpu->SetGyro_X_Offset(100);

    // Verify the result
    if (result != I2C_STATUS_SUCCESS) {
        throw std::runtime_error("SetGyro_X_Offset test failed!");
}
}

// Test case for GetGyro_X_Offset method
/**
 * @brief Gets the instance of the class mpu and calls the function GetGyro_X_Offset.
 * Throws runtime error if the getting is not successful or the returned value doesn't match with the set value.
 * @param offset stores the state of the function call in int16_t type.
 * @return None
 */
void testGetGyro_X_Offset(MPU6050_Driver::MPU6050* mpu) {
    //MPU6050 mpu6050;
    std::cout << "Test function for GetGyro_X_Offset is getting executed" << std::endl;
    // Call the method
    i2c_status_t error;
    int16_t offset = mpu->GetGyro_X_Offset(&error);

    // Verify the result
    if (error != I2C_STATUS_SUCCESS || offset != 100) {
        throw std::runtime_error("GetGyro_X_Offset test failed!");
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
    MPU6050_Driver::MPU6050 MPU6050(&MPU6050_I2C_Callback, &MPU6050Callback, 4);

    //Execute test case
        testSetGyro_X_Offset(&MPU6050);
        testGetGyro_X_Offset(&MPU6050);
    std::cout << "All Gyro_X_Offset tests passed!" << std::endl;
    MPU6050.begin();
    return 0;
}
