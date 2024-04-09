/**
 * @file    mpu_testing.cpp
 * @author  Adam J. Englebright
 * @date    03.03.2024
 * @brief   This file constains a program for testing I2C comms with the MPU6050.
 *
 * Copyright 2024 Adam J. Englebright <adamenglebright@rocketmail.com>
 *
 */

#include <cmath>
#include <limits>
#include <thread>
#include <chrono>
#include <iostream>
#include "../lib/pid/pid.h"
#include "../lib/mpu6050/mpu6050.h"
#include "../lib/i2c_interface/smbus_i2c_if.h"
#include "../lib/ina260/ina260.h"
#include "../lib/MotorDriver/MotorDriver.h"


/**
 * @brief Implementation of the MPU6050Interface. Simply reads data from the MPU
 * and displays it for testing.
 */
class MPU6050_Feedback : public MPU6050_Driver::MPU6050Interface
{
public:
  /**
   * @brief Constructor assigning values.
   */
  MPU6050_Feedback(float _radius, float _samplePeriod)
    : radius(_radius), samplePeriod(_samplePeriod) {}

  /**
   * @brief MPU6050 callback implementation. Takes the sample data and calculates the angular position of the cup holder,
   * which is then printed.
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

    // Print data.
    std::cout << "Accel X = " << sample.ax << " g, Gyro X = " << sample.gx << " deg/s" << std::endl;
    std::cout << "Accel Y = " << sample.ay << " g, Gyro Y = " << sample.gy << " deg/s" << std::endl;
    std::cout << "Accel Z = " << sample.az << " g, Gyro Z = " << sample.gz << " deg/s" << std::endl;
    std::cout << "Calculated angular postion = " << angularPos << " rad" << std::endl << std::endl;
    std::cout << "Calculated angular postion = " << angularPosDeg << " deg" << std::endl << std::endl;
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

  // For an explanation of below, see https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf, page 12.
  float MPU_SamplePeriod;
  if (MPU_DLPFconf == MPU6050_Driver::DLPF_t::BW_260Hz || MPU_DLPFconf == MPU6050_Driver::DLPF_t::RESERVED)
    MPU_SamplePeriod = (1.0 + (float)MPU_SRdiv) / 8000.0;
  else
    MPU_SamplePeriod = (1.0 + (float)MPU_SRdiv) / 1000.0;

  // Radius from axis of ratation to MPU chip (need to actually measure this):
  float radius = 0.15;

  // I2C device files and addresses for MPU:
  std::string MPU_i2cFile = "/dev/i2c-1";
  uint8_t MPU_Address = MPU6050_ADDRESS;

  // Initialise MPU6050 object with callback for printing data, and I2C callback for communication.
  MPU6050_Feedback MPU6050Callback(radius, MPU_SamplePeriod);
  SMBUS_I2C_IF MPU6050_I2C_Callback;
  MPU6050_I2C_Callback.Init_I2C(MPU_Address, MPU_i2cFile);
  MPU6050_Driver::MPU6050 MPU6050(&MPU6050_I2C_Callback, &MPU6050Callback, 17);

  // Setup settings on MPU over i2c.
  MPU6050.InitializeSensor(MPU_GyroScale, MPU_AccelScale, MPU_DLPFconf, MPU_SRdiv, MPU_INTconf, MPU_INTenable, 0, 1); // Given the MPU's orientation, there should be 1g in the Y axis at initalisaton

  // Start data aquisition and processing from the MPU.
  MPU6050.begin();

  // Sleep this thread forever.
  while (true)
    std::this_thread::sleep_for(std::chrono::seconds(86400)); // Check once a day.
}
