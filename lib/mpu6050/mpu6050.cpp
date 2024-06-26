/**
 ******************************************************************************
 * @file    mpu6050.cpp
 * @author  Ali Batuhan KINDAN
 * @author  Adam Englebright
 * @date    07.03.2024
 * @brief   This file constains MPU6050 driver implementation.
 *
 * MIT License
 *
 * Copyright (c) 2022 Ali Batuhan KINDAN
 * Copyright (c) 2024 Adam Englebright
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "mpu6050.h"
#include <gpiod.hpp>

namespace MPU6050_Driver {

/**
 * @brief  Class constructor. In order to make the class communicate with sensor
 * user should pass a valid I2C_Interface class instance!
 * @param  comInterface I2C interface pointer
 * @param  mpuInterface MPU6050 interface pointer
 * @param  _gpioPin GPIO pin that will listen for interrupts from the MPU.
 * @retval none
 */
MPU6050::MPU6050(I2C_Interface *comInterface, MPU6050Interface *mpuInterface,
                 gpiod::line::offset _gpioPin)
    : gpioPin(_gpioPin) {
  /* assign internal interface pointers if given is not null! */
  if (comInterface)
    this->i2c = comInterface;
  if (mpuInterface)
    this->mpu6050cb = mpuInterface;
}

/**
 * @brief  This method wakes up the sensor and configures the accelerometer and
 * gyroscope full scale renges with given parameters. It also configures the
 * DLPF, sample rate divider, interrput configuration, and also calibrates
 * the accelerometers and gyros.
 * It returns the result of the process.
 * @param  gyroScale Gyroscope scale value to be set
 * @param  accelScale Accelerometer scale value to be set
 * @param  DLPFconf Digital Low Pass Filter configuration
 * @param  SRdiv Sample rate divider
 * @param  INTconf Interrupt configuration
 * @param  INTenable Interrput types enabled
 * @param  accelCalX Target acceleration in the X axis for calibration (units g)
 * @param  accelCalY Target acceleration in the Y axis for calibration (units g)
 * @param  accelCalZ Target acceleration in the Z axis for calibration (units g)
 * @param  gyroCalX Target angular velocity in the X axis for calibration
 * (units deg/s)
 * @param  gyroCalY Target angular velocity in the Y axis for calibration
 * (units deg/s)
 * @param  gyroCalZ Target angular velocity in the Z axis for calibration
 * (units deg/s)
 * @retval i2c_status_t Success status
 */
i2c_status_t MPU6050::InitializeSensor(
    Gyro_FS_t gyroScale, Accel_FS_t accelScale, DLPF_t DLPFconf, uint8_t SRdiv,
    uint8_t INTconf, uint8_t INTenable, float accelCalX, float accelCalY,
    float accelCalZ, float gyroCalX, float gyroCalY, float gyroCalZ) {
  accelFSRange = accelScale;
  gyroFSRange = gyroScale;

  i2c_status_t result = WakeUpSensor();
  //std::cout << "Woken" << std::endl;
  if (result == I2C_STATUS_SUCCESS)
    result = SetGyroFullScale(gyroScale);

  //std::cout << "full gyro" << std::endl;
  if (result == I2C_STATUS_SUCCESS)
    result = SetAccelFullScale(accelScale);

  //std::cout << "full accel" << std::endl;
  if (result == I2C_STATUS_SUCCESS)
    result = SetSensor_DLPF_Config(DLPFconf);

  //std::cout << "dlpf conf" << std::endl;
  if (result == I2C_STATUS_SUCCESS)
    result = SetSensor_InterruptPinConfig(INTconf);

  //std::cout << "SRdiv set" << std::endl;
  if (result == I2C_STATUS_SUCCESS)
    result = SetGyro_SampleRateDivider(SRdiv);

  //std::cout << "int conf" << std::endl;
  if (result == I2C_STATUS_SUCCESS)
    result = SetSensor_InterruptEnable(INTenable);

  /* The below calibration methods did not work correctly for us, and we found the factory calibration
   * to be sufficiently accurate, so we have not used them.
   */
  
  //std::cout << "int en" << std::endl;
  //    if(result == I2C_STATUS_SUCCESS)
  // result = Calibrate_Accel_Registers(accelCalX, accelCalY, accelCalZ);

  //std::cout << "cal accel" << std::endl;
  //    if(result == I2C_STATUS_SUCCESS)
  // result = Calibrate_Gyro_Registers(gyroCalX, gyroCalY, gyroCalZ);

  //std::cout << "cal gyro" << std::endl;
  return result;
}

/** Begin dataAquisition() method in a separate thread in a running state. */
void MPU6050::begin(void) {
  dataAquisitionRunning = true;
  dataAquisitionThread = std::thread(&MPU6050::dataAquisition, this);
}

/** Stop dataAquisition() method and close the thread running it. */
void MPU6050::end(void) {
  dataAquisitionRunning = false;
  dataAquisitionThread.join();
}

/**
 * @brief  This method will read all raw sensor data (accel, gyro, temp) into
 * the rawData array.
 * @param  None
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::ReadAllRawData(void) {
  // Read all sensor data (big endian) in one block read and arrange
  // appropriately (little endian) into rawData array.
  uint8_t tmpArray[sizeof(rawData)];
  i2c_status_t err;
  err = i2c->ReadRegisterBlock(MPU6050_ADDRESS, Sensor_Regs::ACCEL_X_OUT_H,
                               sizeof(rawData), tmpArray);

  if (err != I2C_STATUS_SUCCESS)
    return err;

  rawData[0] = ((int16_t)tmpArray[0] << 8) | (int16_t)tmpArray[1];
  rawData[1] = ((int16_t)tmpArray[2] << 8) | (int16_t)tmpArray[3];
  rawData[2] = ((int16_t)tmpArray[4] << 8) | (int16_t)tmpArray[5];
  rawData[3] = ((int16_t)tmpArray[6] << 8) | (int16_t)tmpArray[7];
  rawData[4] = ((int16_t)tmpArray[8] << 8) | (int16_t)tmpArray[9];
  rawData[5] = ((int16_t)tmpArray[10] << 8) | (int16_t)tmpArray[11];
  rawData[6] = ((int16_t)tmpArray[12] << 8) | (int16_t)tmpArray[13];
  return err;
}

/**
 * @brief  This method wakes the sensor up by cleraing the
 * MPU6050_Regs::PWR_MGMT_1 BIT_SLEEP. Power management 1 sensors default values
 * is 0x40 so it will be in sleep mode when it's powered up.
 * @param  none
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::WakeUpSensor(void) {
  return i2c->WriteRegisterBit(MPU6050_ADDRESS, Sensor_Regs::PWR_MGMT_1,
                               Regbits_PWR_MGMT_1::BIT_SLEEP, false);
}

/**
 * @brief  This method resets the sensor by simply setting the
 * MPU6050_Regs::PWR_MGMT_1 Device_Reset bit. After the sensor reset this bit
 * will be cleared automatically.
 * TODO: Can be modify later to check the Device_Reset bit is clear after the
 * reset in order to make it safer (for this we probably need an interface for
 * platform delay function).
 * @param  none
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::ResetSensor(void) {
  return i2c->WriteRegisterBit(MPU6050_ADDRESS, Sensor_Regs::PWR_MGMT_1,
                               Regbits_PWR_MGMT_1::BIT_DEVICE_RESET, true);
}

/**
 * @brief  This method used for configuring the gyroscope full scale range.
 * Check gyro_full_scale_range_t for available scales.
 * @param  gyroScale Gyroscope scale value to be set
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetGyroFullScale(Gyro_FS_t gyroScale) {
  return i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::GYRO_CONFIG,
                            ((uint8_t)gyroScale << 3));
}

/**
 * @brief  This method used for getting the gyroscope full scale range.
 * Check gyro_full_scale_range_t for available scales. It basically reads the
 * Gyro configuration register and returns the full scale range.
 * @param  error Result of the sensor reading process
 * @retval gyro_full_scale_range_t
 */
Gyro_FS_t MPU6050::GetGyroFullScale(i2c_status_t *error) {
  uint8_t gyroConfig =
      i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::GYRO_CONFIG, error);
  return (Gyro_FS_t)((gyroConfig >> 3) & 0x03);
}

/**
 * @brief  This method used for getting the latest gyroscope X axis RAW value
 * from the sensor. Make sure that sensor is not in sleeping mode and gyroscope
 * full scale range is set to desired range before reading the values.
 * @param  error Error state of process
 * @retval int16_t X axis RAW gyroscope value
 */
int16_t MPU6050::GetGyro_X_Raw(i2c_status_t *error) {
  int16_t gyroXVal = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::GYRO_X_OUT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    gyroXVal = (gyroXVal << 8) |
               i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::GYRO_X_OUT_L,
                                 error); // assemble higher and lower bytes
    return gyroXVal;
  }

  return 0x00;
}

/**
 * @brief  This method used for getting the latest gyroscope Y axis RAW value
 * from the sensor. Make sure that sensor is not in sleeping mode and gyroscope
 * full scale range is set to desired range before reading the values.
 * @param  error Error state of process
 * @retval int16_t Y axis RAW gyroscope value
 */
int16_t MPU6050::GetGyro_Y_Raw(i2c_status_t *error) {
  int16_t gyroYVal = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::GYRO_Y_OUT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    gyroYVal = (gyroYVal << 8) |
               i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::GYRO_Y_OUT_L,
                                 error); // assemble higher and lower bytes
    return gyroYVal;
  }

  return 0x00;
}

/**
 * @brief  This method used for getting the latest gyroscope Z axis RAW value
 * from the sensor. Make sure that sensor is not in sleeping mode and gyroscope
 * full scale range is set to desired range before reading the values.
 * @param  error Error state of process
 * @retval int16_t Z axis RAW gyroscope value
 */
int16_t MPU6050::GetGyro_Z_Raw(i2c_status_t *error) {
  int16_t gyroZVal = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::GYRO_Z_OUT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    gyroZVal = (gyroZVal << 8) |
               i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::GYRO_Z_OUT_L,
                                 error); // assemble higher and lower bytes
    return gyroZVal;
  }

  return 0x00;
}

/**
 * @brief  This method used for configuring the accelerometer full scale range.
 * Check accel_full_scale_range_t for available scales.
 * @param  accelScale Accelerometer scale value to be set
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetAccelFullScale(Accel_FS_t accelScale) {
  return i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::ACCEL_CONFIG,
                            ((uint8_t)accelScale << 3));
}

/**
 * @brief  This method used for getting the acceleromteter full scale range.
 * Check accel_full_scale_range_t for available scales. It basically reads the
 * Accel configuration register and returns the full scale range.
 * @param  error Result of the sensor reading process
 * @retval accel_full_scale_range_t
 */
Accel_FS_t MPU6050::GetAccelFullScale(i2c_status_t *error) {
  uint8_t accelConfig =
      i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::ACCEL_CONFIG, error);
  return (Accel_FS_t)((accelConfig >> 3) & 0x03);
}

/**
 * @brief  This method used for getting the latest accelerometer X axis RAW
 * value from the sensor. Make sure that sensor is not in sleeping mode and
 * accelerometer full scale range is set to desired range, before reading the
 * values.
 * @param  error Error state of process
 * @retval int16_t X axis RAW acceleration value
 */
int16_t MPU6050::GetAccel_X_Raw(i2c_status_t *error) {
  int16_t accelXVal = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::ACCEL_X_OUT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    accelXVal = (accelXVal << 8) |
                i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::ACCEL_X_OUT_L,
                                  error); // assemble higher and lower bytes
    return accelXVal;
  }

  return 0x00;
}

/**
 * @brief  This method used for getting the latest accelerometer Y axis RAW
 * value from the sensor. Make sure that sensor is not in sleeping mode and
 * accelerometer full scale range is set to desired range, before reading the
 * values.
 * @param  error Error state of process
 * @retval int16_t Y axis RAW acceleration value
 */
int16_t MPU6050::GetAccel_Y_Raw(i2c_status_t *error) {
  int16_t accelYVal = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::ACCEL_Y_OUT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    accelYVal = (accelYVal << 8) |
                i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::ACCEL_Y_OUT_L,
                                  error); // assemble higher and lower bytes
    return accelYVal;
  }

  return 0x00;
}

/**
 * @brief  This method used for getting the latest accelerometer Z axis RAW
 * value from the sensor. Make sure that sensor is not in sleeping mode and
 * accelerometer full scale range is set to desired range, before reading the
 * values.
 * @param  error Error state of process
 * @retval int16_t Z axis RAW acceleration value
 */
int16_t MPU6050::GetAccel_Z_Raw(i2c_status_t *error) {
  int16_t accelZVal = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::ACCEL_Z_OUT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    accelZVal = (accelZVal << 8) |
                i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::ACCEL_Z_OUT_L,
                                  error); // assemble higher and lower bytes
    return accelZVal;
  }

  return 0x00;
}

/**
 * @brief  This method used for getting the latest temperature value from the
 * sensor. scale range is set to desired range, before reading the values.
 * @param  error Error state of process
 * @retval float Temperature in celcius-degrees
 */
float MPU6050::GetTemperature_Celcius(i2c_status_t *error) {
  int16_t sensorTemp = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::TEMP_OUT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    sensorTemp = (sensorTemp << 8) |
                 i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::TEMP_OUT_L,
                                   error); // assemble higher and lower bytes
    return (sensorTemp / 340.0 + 36.53f);
  }

  return 0x00;
}

/**
 * @brief  This method used for setting the gyroscope X axis offset value.
 * Offset is using in the sensor calibration routine.
 * @param offset
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetGyro_X_Offset(int16_t offset) {
  i2c_status_t result = i2c->WriteRegister(
      MPU6050_ADDRESS, Sensor_Regs::XG_OFFS_USR_H, (offset >> 8));
  if (result == I2C_STATUS_SUCCESS) {
    result = i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::XG_OFFS_USR_L,
                                (offset & 0x00FF));
  }
  return result;
}

/**
 * @brief  This method used for getting the gyroscope X axis offset value.
 * @param error Result of the operation
 * @retval int16_t
 */
int16_t MPU6050::GetGyro_X_Offset(i2c_status_t *error) {
  int16_t gyroXOffset = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::XG_OFFS_USR_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    gyroXOffset = (gyroXOffset << 8) |
                  i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::XG_OFFS_USR_L,
                                    error); // assemble higher and lower bytes
    return gyroXOffset;
  }

  return 0x00;
}

/**
 * @brief  This method used for setting the gyroscope Y axis offset value.
 * Offset is using in the sensor calibration routine.
 * @param offset
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetGyro_Y_Offset(int16_t offset) {
  i2c_status_t result = i2c->WriteRegister(
      MPU6050_ADDRESS, Sensor_Regs::YG_OFFS_USR_H, (offset >> 8));
  if (result == I2C_STATUS_SUCCESS) {
    result = i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::YG_OFFS_USR_L,
                                (offset & 0x00FF));
  }
  return result;
}

/**
 * @brief  This method used for getting the gyroscope Y axis offset value.
 * @param error Result of the operation
 * @retval int16_t
 */
int16_t MPU6050::GetGyro_Y_Offset(i2c_status_t *error) {
  int16_t gyroYOffset = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::YG_OFFS_USR_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    gyroYOffset = (gyroYOffset << 8) |
                  i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::YG_OFFS_USR_L,
                                    error); // assemble higher and lower bytes
    return gyroYOffset;
  }

  return 0x00;
}

/**
 * @brief  This method used for setting the gyroscope Z axis offset value.
 * Offset is using in the sensor calibration routine.
 * @param offset
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetGyro_Z_Offset(int16_t offset) {
  i2c_status_t result = i2c->WriteRegister(
      MPU6050_ADDRESS, Sensor_Regs::ZG_OFFS_USR_H, (offset >> 8));
  if (result == I2C_STATUS_SUCCESS) {
    result = i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::ZG_OFFS_USR_L,
                                (offset & 0x00FF));
  }
  return result;
}

/**
 * @brief  This method used for getting the gyroscope Z axis offset value.
 * @param error Result of the operation
 * @retval int16_t
 */
int16_t MPU6050::GetGyro_Z_Offset(i2c_status_t *error) {
  int16_t gyroZOffset = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::ZG_OFFS_USR_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    gyroZOffset = (gyroZOffset << 8) |
                  i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::ZG_OFFS_USR_L,
                                    error); // assemble higher and lower bytes
    return gyroZOffset;
  }

  return 0x00;
}

/**
 * @brief  This method used for calibrating the gyroscope registers to given
 * target values. Its VERY important to mention that when you call this method
 * make sure that the sensor is standing statically (no vibrations, no
 * rotations, no movement etc.) otherwise you will end up with wrong calibration
 * values!
 * TODO: Use more detailed return type about the calibration status to inform
 * user about the failure (which step it failed and why etc.).
 * @param targetX target value for gyroscope X axis register
 * @param targetY target value for gyroscope Y axis register
 * @param targetZ target value for gyroscope Z axis register
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::Calibrate_Gyro_Registers(int16_t targetX, int16_t targetY,
                                               int16_t targetZ) {
  i2c_status_t result = I2C_STATUS_NONE;
  Gyro_FS_t gyroRange = GetGyroFullScale(&result);
  if (result != I2C_STATUS_SUCCESS)
    return result;

  /* DPS constant to convert raw register value to the
   * degree per seconds (angular velocity). */
  const float dpsConstant = dpsConstantArr[static_cast<uint8_t>(gyroRange)];
  float sumOfSamples = 0;
  int16_t offsetVal = 0;

  /*
   * Gyro X axis calibration
   */
  for (uint16_t i = 0; i < 1000 && result == I2C_STATUS_SUCCESS; i++) {
    sumOfSamples += GetGyro_X_Raw(&result);
  }
  sumOfSamples *= 0.001f; // get mean value of 1000 readings

  if (result != I2C_STATUS_SUCCESS)
    return result;

  offsetVal =
      (int16_t)(((targetX - sumOfSamples) * dpsConstant) * gyro_offset_1dps);
  result = SetGyro_X_Offset(offsetVal);

  if (result != I2C_STATUS_SUCCESS)
    return result;

  /*
   * Gyro Y axis calibration
   */
  sumOfSamples = 0;
  for (uint16_t i = 0; i < 1000 && result == I2C_STATUS_SUCCESS; i++) {
    sumOfSamples += GetGyro_Y_Raw(&result);
  }
  sumOfSamples *= 0.001f; // get mean value of 1000 readings

  if (result != I2C_STATUS_SUCCESS)
    return result;

  offsetVal =
      (int16_t)(((targetY - sumOfSamples) * dpsConstant) * gyro_offset_1dps);
  result = SetGyro_Y_Offset(offsetVal);

  if (result != I2C_STATUS_SUCCESS)
    return result;

  /*
   * Gyro Z axis calibration
   */
  sumOfSamples = 0;
  for (uint16_t i = 0; i < 1000 && result == I2C_STATUS_SUCCESS; i++) {
    sumOfSamples += GetGyro_Z_Raw(&result);
  }
  sumOfSamples *= 0.001f; // get mean value of 1000 readings

  if (result != I2C_STATUS_SUCCESS)
    return result;

  offsetVal =
      (int16_t)(((targetZ - sumOfSamples) * dpsConstant) * gyro_offset_1dps);
  result = SetGyro_Z_Offset(offsetVal);

  return result;
}

/**
 * @brief  This method returns the DPS (Degree Per Second) coversion value
 * depending on the gyroscope full scale range. DPS value is used to convert raw
 * sensor value to angular velocity for orientation related calculations.
 * @param gyroRange Configured gyro full scale range
 * @retval float
 */
float MPU6050::GetGyro_DPS_Constant(Gyro_FS_t gyroRange) {
  return dpsConstantArr[static_cast<uint8_t>(gyroRange)];
}

/**
 * @brief  This method used for setting the accelerometer X axis offset value.
 * Offset is using in the sensor calibration routine.
 * @param offset
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetAccel_X_Offset(int16_t offset) {
  i2c_status_t result = i2c->WriteRegister(
      MPU6050_ADDRESS, Sensor_Regs::XA_OFFS_USR_H, (offset >> 8));
  if (result == I2C_STATUS_SUCCESS) {
    result = i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::XA_OFFS_USR_L,
                                (offset & 0x00FF));
  }
  return result;
}

/**
 * @brief  This method used for getting the accelerometer X axis offset value.
 * @param error Result of the operation
 * @retval int16_t
 */
int16_t MPU6050::GetAccel_X_Offset(i2c_status_t *error) {
  int16_t accelXOffset = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::XA_OFFS_USR_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    accelXOffset =
        (accelXOffset << 8) |
        i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::XA_OFFS_USR_L,
                          error); // assemble higher and lower bytes
    return accelXOffset;
  }

  return 0x00;
}

/**
 * @brief  This method used for setting the accelerometer Y axis offset value.
 * Offset is using in the sensor calibration routine.
 * @param offset
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetAccel_Y_Offset(int16_t offset) {
  i2c_status_t result = i2c->WriteRegister(
      MPU6050_ADDRESS, Sensor_Regs::YA_OFFS_USR_H, (offset >> 8));
  if (result == I2C_STATUS_SUCCESS) {
    result = i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::YA_OFFS_USR_L,
                                (offset & 0x00FF));
  }
  return result;
}

/**
 * @brief  This method used for getting the accelerometer Y axis offset value.
 * @param error Result of the operation
 * @retval int16_t
 */
int16_t MPU6050::GetAccel_Y_Offset(i2c_status_t *error) {
  int16_t accelYOffset = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::YA_OFFS_USR_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    accelYOffset =
        (accelYOffset << 8) |
        i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::YA_OFFS_USR_L,
                          error); // assemble higher and lower bytes
    return accelYOffset;
  }

  return 0x00;
}

/**
 * @brief  This method used for setting the accelerometer Z axis offset value.
 * Offset is using in the sensor calibration routine.
 * @param offset
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetAccel_Z_Offset(int16_t offset) {
  i2c_status_t result = i2c->WriteRegister(
      MPU6050_ADDRESS, Sensor_Regs::ZA_OFFS_USR_H, (offset >> 8));
  if (result == I2C_STATUS_SUCCESS) {
    result = i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::ZA_OFFS_USR_L,
                                (offset & 0x00FF));
  }
  return result;
}

/**
 * @brief  This method used for getting the accelerometer Z axis offset value.
 * @param error Result of the operation
 * @retval int16_t
 */
int16_t MPU6050::GetAccel_Z_Offset(i2c_status_t *error) {
  int16_t accelZOffset = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::ZA_OFFS_USR_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    accelZOffset =
        (accelZOffset << 8) |
        i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::ZA_OFFS_USR_L,
                          error); // assemble higher and lower bytes
    return accelZOffset;
  }

  return 0x00;
}

/**
 * @brief  This method used for calibrating the accelerometer registers to given
 * target values. Even if the official calibration method in the invensense
 * application notes are tried, it didnt work as expected. So there is another
 * method implemented to calibrate accelerometer registers automatically. It
 * works with the similar concept of binary search algorithm (setting a range
 * and narrowing on each step).
 * @param targetX_MG target value for accelerometer X axis register in MG so 1.0f
 * means 1G
 * @param targetY_MG target value for accelerometer Y axis register in MG
 * @param targetZ_MG target value for accelerometer Z axis register in MG
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::Calibrate_Accel_Registers(float targetX_MG,
                                                float targetY_MG,
                                                float targetZ_MG) {
  i2c_status_t result = I2C_STATUS_NONE;
  Accel_FS_t accelRange = GetAccelFullScale(&result);
  if (result != I2C_STATUS_SUCCESS)
    return result;

  /* MG constant to convert raw register value to the
   * gravity (9.81 m/s2). */
  const float mgConstant = mgCostantArr[static_cast<uint8_t>(accelRange)];

  /* Some constants for our calibration routine to modify easily when needed. */
  const int16_t calibrationRangeHigh = 4096;
  const int16_t calibrationRangeLow = -calibrationRangeHigh;
  const uint8_t calibrationSteps =
      13; // if the calibrationRangeHigh = 2^n so this will be (n +1)
  const int tolerance =
      5; // acceptable tolerance between target and mean value of samples

  float meanOfNSamples = 0;
  int16_t regExpected = 0;
  int16_t diff = 0;
  int16_t high = calibrationRangeHigh;
  int16_t low = calibrationRangeLow;
  int16_t currentOffsetVal = 0;

  result = SetAccel_X_Offset(0); // set the offset to 0 first

  if (result != I2C_STATUS_SUCCESS)
    return result;

  result = SetAccel_Y_Offset(0); // set the offset to 0 first

  if (result != I2C_STATUS_SUCCESS)
    return result;

  result = SetAccel_Z_Offset(0); // set the offset to 0 first

  if (result != I2C_STATUS_SUCCESS)
    return result;

  /*
   * Accel X axis calibration
   */
  regExpected = targetX_MG / mgConstant;

  /* Get the initial deviation from our target value after reseting the offset
   * register */
  meanOfNSamples = 0;
  for (uint8_t i = 0; i < 100 && result == I2C_STATUS_SUCCESS; i++) {
    meanOfNSamples += GetAccel_X_Raw(&result);
  }

  if (result != I2C_STATUS_SUCCESS)
    return result;

  meanOfNSamples *= 0.01;
  diff = regExpected - meanOfNSamples;

  /* Limit our ranges depending on the initial results. So we
   * are either work on negative or positive range during the
   * calibration steps. */
  if (diff < 0)
    high = 0;
  else
    low = 0;

  /* Start N steps of calibration. This method is very similar to binary search
   * algorightm in sorted list. On every iteration we are setting the offset
   * register to a middle value of our high and low range then we read 100
   * samples from Accelerometer and compare our target and mean value of the
   * samples. Depending on the result we are narrowing our high and low limits
   * and repeat... */
  for (uint8_t step = 0; step < calibrationSteps; step++) {
    /* Update offset register! */
    currentOffsetVal = (int16_t)((high + low) / 2.0f);
    result = SetAccel_X_Offset(currentOffsetVal);
    if (result != I2C_STATUS_SUCCESS)
      return result;

    /* Take 100 samples and compare with target */
    meanOfNSamples = 0;
    for (uint8_t i = 0; i < 100 && result == I2C_STATUS_SUCCESS; i++) {
      meanOfNSamples += GetAccel_X_Raw(&result);
    }

    if (result != I2C_STATUS_SUCCESS)
      return result;

    meanOfNSamples *= 0.01;
    diff = regExpected - meanOfNSamples;

    /* Quick math.abs to check if difference is in tolerance level.
     * If yes abort calibration for this axis its done already! */
    if ((diff & 0x8000 ? -diff : diff) < tolerance)
      break;

    /* Update ranges! */
    if (diff < 0)
      high = currentOffsetVal;
    else
      low = currentOffsetVal;
  } // for calibrationSteps

  /*
   * Accel Y axis calibration (TODO: unfortunately bad practice of code reusing
   * but keep it for now!)
   */
  high = calibrationRangeHigh;
  low = calibrationRangeLow;
  currentOffsetVal = 0;

  regExpected = targetY_MG / mgConstant;

  /* Get the initial deviation from our target value after reseting the offset
   * register */
  meanOfNSamples = 0;
  for (uint8_t i = 0; i < 100 && result == I2C_STATUS_SUCCESS; i++) {
    meanOfNSamples += GetAccel_Y_Raw(&result);
  }

  if (result != I2C_STATUS_SUCCESS)
    return result;

  meanOfNSamples *= 0.01;
  diff = regExpected - meanOfNSamples;

  /* Limit our ranges depending on the initial results. So we
   * are either work on negative or positive range during the
   * calibration steps. */
  if (diff < 0)
    high = 0;
  else
    low = 0;

  /* Start N steps of calibration. This method is very similar to binary search
   * algorightm in sorted list. On every iteration we are setting the offset
   * register to a middle value of our high and low range then we read 100
   * samples from Accelerometer and compare our target and mean value of the
   * samples. Depending on the result we are narrowing our high and low limits
   * and repeat... */
  for (uint8_t step = 0; step < calibrationSteps; step++) {
    /* Update offset register! */
    currentOffsetVal = (int16_t)((high + low) / 2.0f);
    result = SetAccel_Y_Offset(currentOffsetVal);
    if (result != I2C_STATUS_SUCCESS)
      return result;

    /* Take 100 samples and compare with target */
    meanOfNSamples = 0;
    for (uint8_t i = 0; i < 100 && result == I2C_STATUS_SUCCESS; i++) {
      meanOfNSamples += GetAccel_Y_Raw(&result);
    }

    if (result != I2C_STATUS_SUCCESS)
      return result;

    meanOfNSamples *= 0.01;
    diff = regExpected - meanOfNSamples;

    /* Quick math.abs to check if difference is in tolerance level.
     * If yes abort calibration for this axis its done already! */
    if ((diff & 0x8000 ? -diff : diff) < tolerance)
      break;

    /* Update ranges! */
    if (diff < 0)
      high = currentOffsetVal;
    else
      low = currentOffsetVal;
  } // for calibrationSteps

  /*
   * Accel Z axis calibration (TODO: unfortunately bad practice of code reusing
   * but keep it for now!)
   */
  high = calibrationRangeHigh;
  low = calibrationRangeLow;
  currentOffsetVal = 0;

  regExpected = targetZ_MG / mgConstant;

  /* Get the initial deviation from our target value after reseting the offset
   * register */
  meanOfNSamples = 0;
  for (uint8_t i = 0; i < 100 && result == I2C_STATUS_SUCCESS; i++) {
    meanOfNSamples += GetAccel_Z_Raw(&result);
  }

  if (result != I2C_STATUS_SUCCESS)
    return result;

  meanOfNSamples *= 0.01;
  diff = regExpected - meanOfNSamples;

  /* Limit our ranges depending on the initial results. So we
   * are either work on negative or positive range during the
   * calibration steps. */
  if (diff < 0)
    high = 0;
  else
    low = 0;

  /* Start N steps of calibration. This method is very similar to binary search
   * algorightm in sorted list. On every iteration we are setting the offset
   * register to a middle value of our high and low range then we read 100
   * samples from Accelerometer and compare our target and mean value of the
   * samples. Depending on the result we are narrowing our high and low limits
   * and repeat... */
  for (uint8_t step = 0; step < calibrationSteps; step++) {
    /* Update offset register! */
    currentOffsetVal = (int16_t)((high + low) / 2.0f);
    result = SetAccel_Z_Offset(currentOffsetVal);
    if (result != I2C_STATUS_SUCCESS)
      return result;

    /* Take 100 samples and compare with target */
    meanOfNSamples = 0;
    for (uint8_t i = 0; i < 100 && result == I2C_STATUS_SUCCESS; i++) {
      meanOfNSamples += GetAccel_Z_Raw(&result);
    }

    if (result != I2C_STATUS_SUCCESS)
      return result;

    meanOfNSamples *= 0.01;
    diff = regExpected - meanOfNSamples;

    /* Quick math.abs to check if difference is in tolerance level.
     * If yes abort calibration for this axis its done already! */
    if ((diff & 0x8000 ? -diff : diff) < tolerance)
      break;

    /* Update ranges! */
    if (diff < 0)
      high = currentOffsetVal;
    else
      low = currentOffsetVal;
  } // for calibrationSteps

  return result;
}

/**
 * @brief  This method returns the MG (Gravity) coversion value depending on
 * the accelerometer full scale range. MG value is used to convert raw sensor
 * value to Gravity for acceleration related calculations.
 * @param accelRange Configured accelerometer full scale range
 * @retval float
 */
float MPU6050::GetAccel_MG_Constant(Accel_FS_t accelRange) {
  return mgCostantArr[static_cast<uint8_t>(accelRange)];
}

/**
 * @brief This function sets the gyroscope sample rate divider. Once the sample rate divider set, actual sample rate
 *        can be found with this formula:
 *        Actual sample rate = Gyroscope Output Rate / (1 + sampleRate)
 *        Keep in mind that Gyroscope Output Rate = 8kHz when the DLPF (digital low pass filter) is disabled
 *        (DLPF_CFG = 0 or 7), and 1kHz when the DLPF is enabled. Accel sample rate is constantly 1 kHz.
 * @param sampleRate Gyroscope sample rate divider.     
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetGyro_SampleRateDivider(uint8_t sampleRate) {
  return i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::SMPRT_DIV,
                            sampleRate);
}

/**
 * @brief This function gets the gyroscope sample rate divider.
 *        Actual sample rate = Gyroscope Output Rate / (1 + sampleRate)
 *        Keep in mind that Gyroscope Output Rate = 8kHz when the DLPF (digital
 * low pass filter) is disabled (DLPF_CFG = 0 or 7), and 1kHz when the DLPF is
 * enabled. Accel sample rate is constantly 1 kHz.
 * @param error Result of the operation
 * @retval uint8_t
 */
uint8_t MPU6050::GetGyro_SampleRateDivider(i2c_status_t *error) {
  return i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::SMPRT_DIV, error);
}

/**
 * @brief This function sets the sensor digital low pass filter values. Tighter
 * bandwitdh configs will generate more delay on the sensor outputs (check
 * sensor datasheet). Keep in mind that default Gyroscope sample rate is 8 kHz
 * but if we set DLPF config different than 0 it will be 1 kHz by default unless
 * if we make an extra configuration to Sample Rate Divider.
 * @param dlpfConfig Digital low pass filter configuration value
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetSensor_DLPF_Config(DLPF_t dlpfConfig) {
  i2c_status_t error = I2C_STATUS_NONE;
  /* This register also have EXT_SYNC config, so only set the DLPF part! */
  uint8_t currentRegVal =
      i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::CONFIG, &error);
  if (error == I2C_STATUS_SUCCESS) {
    currentRegVal &=
        (~0x07); // clear the DLPF section from the current register value
    error = i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::CONFIG,
                               (uint8_t)dlpfConfig | currentRegVal);
  }

  return error;
}

/**
 * @brief This function gets the current sensor digital low pass filter
 * configuration.
 * @param error Result of the operation
 * @retval dlpf_config_t
 */
DLPF_t MPU6050::GetSensor_DLPF_Config(i2c_status_t *error) {
  /* get only the first 3 bit of the register */
  return (
      DLPF_t)(i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::CONFIG, error) &
              0x07);
}

/**
 * @brief This function gets the current sensor sample rate. In order to do
 * this, method reads Sample Rate Divider (0x19) and DLPF Config (0x1A)
 * registers.
 * @param error Result of the operation
 * @retval float Current sample rate in Hz
 */
float MPU6050::GetSensor_CurrentSampleRate_Hz(i2c_status_t *error) {
  uint8_t sampleRateDivider = GetGyro_SampleRateDivider(error);
  if (*error != I2C_STATUS_SUCCESS)
    return 0x00;

  const uint8_t dlpfConfig = static_cast<uint8_t>(GetSensor_DLPF_Config(error));
  if (*error != I2C_STATUS_SUCCESS)
    return 0x00;

  /* if dlpf config is disabled (0 or 7) then sample rate is 8 kHz otherwise 1
   * kHz */
  const float gyroDefaultOutRateHz =
      (dlpfConfig == static_cast<uint8_t>(DLPF_t::BW_260Hz) ||
       dlpfConfig == static_cast<uint8_t>(DLPF_t::RESERVED))
          ? 8000
          : 1000;
  return gyroDefaultOutRateHz / (1 + sampleRateDivider);
}

/**
 * @brief This function gets the number of bytes written in the sensor FIFO
 * buffers.
 * @param error Result of the operation
 * @retval uint16_t Number of samples in the FIFO buffer in bytes
 */
uint16_t MPU6050::GetSensor_FIFOCount(i2c_status_t *error) {
  uint16_t fifoCount = i2c->ReadRegister(
      MPU6050_ADDRESS, Sensor_Regs::FIFO_COUNT_H, error); // higher 8 bits
  if (*error == I2C_STATUS_SUCCESS) {
    fifoCount = (fifoCount << 8) |
                i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::FIFO_COUNT_L,
                                  error); // assemble higher and lower bytes
    return fifoCount;
  }

  return 0x00;
}

/**
 * @brief This function gets the INT_ENABLE register value.
 * @param error Result of the operation
 * @retval uint8_t Enabled/Disabled sensor interrupts. Use Regbits_INT_ENABLE
 * namespace as bitmask to check enabled interrupts.
 */
uint8_t MPU6050::GetSensor_InterruptEnable(i2c_status_t *error) {
  return i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::INT_ENABLE, error);
}

/**
 * @brief This function sets the sensor INT_ENABLE register with given value.
 * @param enabledInterrupts Enabled/Disabled sensor interrupts. Use
 * Regbits_INT_ENABLE namespace.
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetSensor_InterruptEnable(uint8_t enabledInterrupts) {
  return i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::INT_ENABLE,
                            enabledInterrupts);
}

/**
 * @brief This function gets the sensor FIFO configuration. Use Regbits_FIFO_EN
 * as bitmask to check which samples enabled in the FIFO reading.
 * @param error Result of the operation
 * @retval uint8_t Sensor fifo configuration value, use Regbits_FIFO_EN to check
 * fifo config.
 */
uint8_t MPU6050::GetSensor_FIFO_Config(i2c_status_t *error) {
  return i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::FIFO_EN, error);
}

/**
 * @brief This function sets the sensor FIFO configuration.
 * @param fifoConfigVal FIFO config value, use Regbits_FIFO_EN as bitmask to
 * configure.
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetSensor_FIFO_Config(uint8_t fifoConfigVal) {
  return i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::FIFO_EN,
                            fifoConfigVal);
}

/**
 * @brief This function gets the sensor FIFO enable bit in USER_CTRL register.
 * @param error Result of the operation
 * @retval bool True if FIFO enabled
 */
bool MPU6050::GetSensor_FIFO_Enable(i2c_status_t *error) {
  return i2c->ReadRegisterBit(MPU6050_ADDRESS, Sensor_Regs::USER_CTRL,
                              Regbits_USER_CTRL::BIT_FIFO_EN, error);
}

/**
 * @brief This function sets the sensor FIFO enable bit in USER_CTRL register.
 * @param state State of the FIFO to be set. True if it will be enabled.
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetSensor_FIFO_Enable(bool state) {
  return i2c->WriteRegisterBit(MPU6050_ADDRESS, Sensor_Regs::USER_CTRL,
                               Regbits_USER_CTRL::BIT_FIFO_EN, state);
}

/**
 * @brief This function resets the sensor FIFO.
 * @param none
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::Reset_Sensor_FIFO(void) {
  return i2c->WriteRegisterBit(MPU6050_ADDRESS, Sensor_Regs::USER_CTRL,
                               Regbits_USER_CTRL::BIT_FIFO_RESET, true);
}

/**
 * @brief This function gets the sensor interrput status (INT_STATUS) register.
 * @param error Result of the operation
 * @retval uint8_t Register value.
 */
uint8_t MPU6050::GetSensor_InterruptStatus(i2c_status_t *error) {
  return i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::INT_STATUS, error);
}

/**
 * @brief This function reads 1 byte from sensor FIFO data register.
 * @param error Result of the operation.
 * @retval uint8_t FIFO data.
 */
uint8_t MPU6050::GetSensor_FIFO_Data(i2c_status_t *error) {
  return i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::FIFO_R_W, error);
}

/**
 * @brief This function returns sensor interrupt pin config register value.
 * @param error Result of the operation.
 * @retval uint8_t Interrupt pin config register value.
 */
uint8_t MPU6050::GetSensor_InterruptPinConfig(i2c_status_t *error) {
  return i2c->ReadRegister(MPU6050_ADDRESS, Sensor_Regs::INT_PIN_CFG, error);
}

/**
 * @brief This function sets the sensor interrupt pin config register.
 * @param intPinConfig interrput pin config value to set
 * @retval i2c_status_t
 */
i2c_status_t MPU6050::SetSensor_InterruptPinConfig(uint8_t intPinConfig) {
  return i2c->WriteRegister(MPU6050_ADDRESS, Sensor_Regs::INT_PIN_CFG,
                            intPinConfig);
}

/**
 * Enter a while loop dependent on the value of dataAquisitionRunning. In this
 * loop, have blocking IO that will only continue when an interrupt is raised by
 * the MPU6050 on one of the GPIO pins. After continuing, read new data from the
 * MPU6050 and package this data into an instance of the MPU6050Sample struct,
 * then send this struct to the registered mpu6050cb callback for processing.
 */
void MPU6050::dataAquisition(void) {
  // Set up GPIO pin for detecting edges from the MPU6050 interrupt pin.
  // Set values as appropriate.
  const std::filesystem::path chip_path("/dev/gpiochip4");

  // Set up edge event that will block until a rising edge is detected
  auto request =
      gpiod::chip(chip_path)
          .prepare_request()
          .set_consumer("watch-line-value")
          .add_line_settings(gpioPin,
                             gpiod::line_settings()
                                 .set_direction(gpiod::line::direction::INPUT)
                                 .set_edge_detection(gpiod::line::edge::RISING)
			         .set_bias(gpiod::line::bias::DISABLED)) // MPU6050 int pin can push and pull
          .do_request();

  // Create buffer for storing edge events.
  gpiod::edge_event_buffer buffer(1);

  // Create MPU6050Sample struct to store data for transfer to the registered
  // callback.
  MPU6050Sample sample;

  // Start data aquisition loop
  while (dataAquisitionRunning) {
    // Block until edge is detected
    request.read_edge_events(buffer);
    
    // Read raw data from MPU6050
    ReadAllRawData();

    // Store data in sample struct, in float format with proper units.
    sample.ax = rawData[0] * GetAccel_MG_Constant(accelFSRange);
    sample.ay = rawData[1] * GetAccel_MG_Constant(accelFSRange);
    sample.az = rawData[2] * GetAccel_MG_Constant(accelFSRange);

    sample.temp =
        rawData[3] / 340.0f + 36.53f; // Conversion taken from datasheet.

    sample.gx = rawData[4] * GetGyro_DPS_Constant(gyroFSRange);
    sample.gy = rawData[5] * GetGyro_DPS_Constant(gyroFSRange);
    sample.gz = rawData[6] * GetGyro_DPS_Constant(gyroFSRange);

    // Send data to the registered callback.
    mpu6050cb->hasSample(sample);
  }
}

} // namespace MPU6050_Driver
