/**
 * @file    main.cpp
 * @author  Adam J. Englebright
 * @date    30.03.2024
 * @brief   This file constains the main program that controls the anti-quake cup holder.
 *
 * Copyright 2024 Adam J. Englebright <adamenglebright@rocketmail.com>
 *
 */

#include <iostream>
#include "../lib/pid/pid.h"
#include "../lib/mpu6050/mpu6050.h"
#include "../lib/ina260/ina260.h"
#include "../lib/MotorDriver/MotorDriver.h"


/**
 * @brief Implementation of the PID_Interface for the inner PID controller,
 * driving the motor driver.
 */
class PID_MotorDriver : PID_Interface
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
class PID_Position : PID_Interface
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
class INA260_Feedback : INA260_Driver::INA260Interface
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
class MPU6050_Feedback : MPU6050_Driver::MPU6050Interface
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
    float gzUnitsCorrected = sample.gz * 3.14159265358979323846 / 180;
    float ayGrav = ayUnitsCorrected + gzUnitsCorrected * gzUnitsCorrected * radius;

    // Adjust x accel component for tangential acceleration caused by angular acceleration around z axis
    // Watch units. Sample linear acceleration is in g. Convert to m/s^2.
    // Watch units. Sample angular velocity is in deg/s. Convert to rad/s.
    float axUnitsCorrected = sample.ax * 9.80665;
    float axGrav = axUnitsCorrected + ((gzUnitsCorrected - gzPrev) / samplePeriod) * radius;
    gzPrev = gzUnitsCorrected;
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
    std::cout << "Hello World!" << std::endl;
    return 0;
}

