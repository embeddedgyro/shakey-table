/**
 * @file    main.cpp
 * @author  Adam J. Englebright
 * @date    30.03.2024
 * @brief   This file constains the main program that controls the anti-quake cup holder.
 *
 * Copyright 2024 Adam J. Englebright <adamenglebright@rocketmail.com>
 *
 */

#include <cmath>
#include <limits>
#include <thread>
#include <chrono>
#include "../lib/pid/pid.h"
#include "../lib/mpu6050/mpu6050.h"
#include "../lib/mpu6050/smbus_i2c_if.h"
#include "../lib/ina260/ina260.h"
#include "../lib/MotorDriver/MotorDriver.h"


/**
 * @brief Implementation of the PID_Interface for the inner PID controller,
 * driving the motor driver.
 */
class PID_MotorDriver : public PID_Interface
{
public:
  /**
   * @brief Constructor taking and assigning a motor driver object reference.
   * @param _motorDriver The motor driver object.
   */
  PID_MotorDriver(MotorDriver& _motorDriver) : motorDriver(_motorDriver) {}
  
  /**
   * @brief PID controller callback implementation, passing the PID output to the provided motor driver object.
   * @param pidOutput Output of the PID controller passed to the callback.
   */
  virtual void hasOutput(double pidOutput) { motorDriver.setDutyCycle(pidOutput); } // May want to have duty cycle acceleration, rather than setting DC directly

private:
  /**
   * @brief Motor driver object reference attribute.
   */
  MotorDriver& motorDriver;
};


/**
 * @brief Implementation of the PID_Interface for the outer PID controller,
 * controlling position via torque outputs that are sent to the inner PID
 * controller.
 */
class PID_Position : public PID_Interface
{
public:
  /**
   * @brief Constructor taking and assigning a PID controller object reference.
   * @param _pidController The PID controller object.
   */
  PID_Position(PID& _pidController) : pidController(_pidController) {}
  
  /**
   * @brief PID controller callback implementation, passing the PID output to the provided PID controller object.
   * @param pidOutput Output of the PID controller passed to the callback.
   */
  virtual void hasOutput(double pidOutput) override { pidController.setSetpoint(pidOutput); }

private:
  /**
   * @brief PID controller object reference attribute.
   */
  PID& pidController;
};


/**
 * @brief Implementation of the INA260Interface for feedback of current (torque)
 * values as the PV for the inner PID controller driving the motor driver.
 */
class INA260_Feedback : public INA260_Driver::INA260Interface
{
public:
  /**
   * @brief Constructor taking and assigning a PID controller object reference.
   * @param _pidController The PID controller object.
   */
  INA260_Feedback(PID& _pidController) : pidController(_pidController) {}

  /**
   * @brief INA260 callback implementation, passing the measured current (torque) to the provided PID controller object.
   * @param sample Current measured by the INA260 passed to the callback.
   */
  virtual void hasSample(INA260_Driver::INA260Sample& sample) override { pidController.calculate(sample.current); } // May want a scale factor to convert current -> torque (or just adjust PID constants)

private:
  /**
   * @brief PID controller object reference attribute.
   */
  PID& pidController;
};


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
  MPU6050_Feedback(PID& _pidController, float _radius, float _samplePeriod)
    : pidController(_pidController), radius(_radius), samplePeriod(_samplePeriod) {}

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

    // Pass angular position to outer PID controller as PV.
    pidController.calculate(angularPos);
  }

private:
  /**
   * @brief PID controller object reference attribute.
   */
  PID& pidController;

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
  MPU6050_Driver::DLPF_t MPU_DLPFconf = MPU6050_Driver::DLPF_t::BW_260Hz;
  uint8_t MPU_SRdiv = 7;
  uint8_t MPU_INTconf = MPU6050_Driver::Regbits_INT_PIN_CFG::BIT_INT_RD_CLEAR;
  uint8_t MPU_INTenable = MPU6050_Driver::Regbits_INT_ENABLE::BIT_DATA_RDY_EN;

  // For an explanation of bellow, see https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf, page 12.
  float MPU_SamplePeriod;
  if (MPU_DLPFconf == MPU6050_Driver::DLPF_t::BW_260Hz || MPU_DLPFconf == MPU6050_Driver::DLPF_t::RESERVED)
    MPU_SamplePeriod = (1.0 + (float)MPU_SRdiv) / 8000.0;
  else
    MPU_SamplePeriod = (1.0 + (float)MPU_SRdiv) / 1000.0;

  // INA260 settings:
  INA260_Driver::Alert_Conf INA_AlertMode = INA260_Driver::Alert_Conf::CNVR;
  INA260_Driver::Conv_Time INA_VoltConvTime = INA260_Driver::Conv_Time::TU140;
  INA260_Driver::Conv_Time INA_CurrConvTime = INA260_Driver::Conv_Time::TU1100;
  INA260_Driver::Ave_Mode INA_AveragingMode = INA260_Driver::Ave_Mode::AV1;
  INA260_Driver::Op_Mode INA_OperatingMode = INA260_Driver::Op_Mode::CURCONT;

  // Bellow assumes that only current is being measured (i.e. INA_OperatingMode = CURCONT).
  float INA_SamplePeriod;
  switch (INA_CurrConvTime) {
  case INA260_Driver::Conv_Time::TU140:
    INA_SamplePeriod = 140e-6;
  case INA260_Driver::Conv_Time::TU204:
    INA_SamplePeriod = 204e-6;
  case INA260_Driver::Conv_Time::TU332:
    INA_SamplePeriod = 332e-6;
  case INA260_Driver::Conv_Time::TU588:
    INA_SamplePeriod = 588e-6;
  case INA260_Driver::Conv_Time::TU1100:
    INA_SamplePeriod = 1100e-6;
  case INA260_Driver::Conv_Time::TU2116:
    INA_SamplePeriod = 2116e-6;
  case INA260_Driver::Conv_Time::TU4156:
    INA_SamplePeriod = 4156e-6;
  case INA260_Driver::Conv_Time::TU8224:
    INA_SamplePeriod = 8224e-6;
  }

  // Radius from axis of ratation to MPU chip (need to actually measure this):
  float radius = 0.15;

  // I2C device files and addresses for MPU and INA:
  std::string MPU_i2cFile = "/dev/i2c-0";
  std::string INA_i2cFile = "/dev/i2c-1";
  uint8_t MPU_Address = MPU6050_ADDRESS;
  uint8_t INA_Address = INA260_ADDRESS;

  // Initialise motor driver object. Assuming line offset 16 for direction pin is correct for now.
  MotorDriver MD20(16);

  // Initialise inner PID controller with callback using motor driver object.
  // Initially with the PID output being the DC directly, so set min to 0 and max to 1.
  // Initially with Kp=1, Kd=0, and Ki=0.
  PID_MotorDriver innerPIDCallback(MD20);
  PID innerPID(&innerPIDCallback, 0, INA_SamplePeriod, 1, 0, 1, 0, 0);

  // Initialise outer PID controller with callback using the inner PID controller.
  // Initially with the PID output being the required corrective torque, so set no limits on min and max (for a double value).
  // Initially with Kp=1, Kd=0, and Ki=0.
  PID_Position outerPIDCallback(innerPID);
  PID outerPID(&outerPIDCallback, 0, MPU_SamplePeriod, std::numeric_limits<double>::max(), std::numeric_limits<double>::lowest(), 1, 0, 0);

  // Initialise MPU6050 object with callback using the outer PID controller, and I2C callback for communication.
  MPU6050_Feedback MPU6050Callback(outerPID, radius, MPU_SamplePeriod);
  SMBUS_I2C_IF MPU6050_I2C_Callback;
  MPU6050_I2C_Callback.Init_I2C(MPU_Address, MPU_i2cFile);
  MPU6050_Driver::MPU6050 MPU6050(&MPU6050_I2C_Callback, &MPU6050Callback);
  
  // Initialise INA260 object with callback using the inner PID controller, and I2C callback for communication.
  INA260_Feedback INA260Callback(innerPID);
  SMBUS_I2C_IF INA260_I2C_Callback;
  INA260_I2C_Callback.Init_I2C(INA_Address, INA_i2cFile);
  INA260_Driver::INA260 INA260(&INA260_I2C_Callback, &INA260Callback);

  // Setup settings on MPU and INA over i2c.
  MPU6050.InitializeSensor(MPU_GyroScale, MPU_AccelScale, MPU_DLPFconf, MPU_SRdiv, MPU_INTconf, MPU_INTenable);
  INA260.InitializeSensor(INA_AlertMode, INA_VoltConvTime, INA_CurrConvTime, INA_AveragingMode, INA_OperatingMode);

  // Start data aquisition and processing from the MPU and INA.
  MPU6050.begin();
  INA260.begin();

  // Sleep this thread forever.
  while (true)
    std::this_thread::sleep_for(std::chrono::seconds(86400)); // Check once a day.
}

