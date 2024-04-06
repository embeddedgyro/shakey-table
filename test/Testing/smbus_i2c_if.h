/**
  ******************************************************************************
  * @file    smbus_i2c_if.h
  * @author  Adam Englebright
  * @author  Ali Batuhan KINDAN
  * @date    28.02.2024
  * @brief   This file constains the definition I2C interface using the
  * kernel SMBus interface. Based on batu92k's example code for defining
  * the I2C interface using the bcm2835 library:
  * https://github.com/batu92k/MPU6050-Sensor-Library-Cpp/blob/master/examples/RPi_4/bcm2835/bcm2835_i2c_if.h
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
  * The above copyright notice and this permission notice shall be included in all
  * copies or substantial portions of the Software.
  * 
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  */

#include "i2c_interface.h"

#ifndef SMBUS_I2C_IF_H
#define SMBUS_I2C_IF_H

/* I2C interface for kernel SMBus support. */
class SMBUS_I2C_IF : public I2C_Interface
{
public:
/**
  * @brief  I2C peripheral initialization method.
  * @param  slaveAddress adress of the device that will be communicated
  * @retval i2c_status_t
  */
  virtual i2c_status_t Init_I2C(uint8_t slaveAddress, std::string i2cFile) override;

/**
  * @brief  This method will be used for reading the data of the given register from
  * the slave with given address.
  * @param  slaveAddress Slave chip I2C bus address (not used in this RTEP5 implementation, but not removed to avoid needing a large rewrite of the library)
  * @param  regAddress Register address to be read
  * @param  status Pointer for operation status
  * @retval uint8_t Read register value
  */
  uint8_t ReadRegister(uint8_t slaveAddress, uint8_t regAddress, i2c_status_t *status = nullptr) override;

/**
  * @brief  This method will be used for writing gven data to the given register of the slave device 
  * with the given address.
  * @param  slaveAddress Slave chip I2C bus address (not used in this RTEP5 implementation, but not removed to avoid needing a large rewrite of the library)
  * @param  regAddress Register address that the data to be written
  * @param  data Data to be written
  * @retval i2c_status_t
  */
  i2c_status_t WriteRegister(uint8_t slaveAddress, uint8_t regAddress, uint8_t data) override;

  /**
   * @brief  This method will be used to read a block of bytes up to 32 bytes long,
   * starting from the given register of the slave device with the given address.
   * @param  slaveAddress Slave chip I2C bus address
   * @param  regAddress Lowest address of the registers to be read from
   * @param  length Number of bytes to be read
   * @param  data Pointer to the array of bytes to be writen to
   * @retval i2c_status_t
   */
  i2c_status_t ReadRegisterBlock(uint8_t slaveAddress, uint8_t regAddress, uint8_t length, uint8_t *data) override;

  /**
   * @brief  This method will be used to write a block of bytes up to 32 bytes long,
   * starting from the given register of the slave device with the given address.
   * @param  slaveAddress Slave chip I2C bus address
   * @param  regAddress Lowest address of the registers to be writen to
   * @param  length Number of bytes to be writen
   * @param  data Pointer to the array of bytes to be read from
   * @retval i2c_status_t
   */
  i2c_status_t WriteRegisterBlock(uint8_t slaveAddress, uint8_t regAddress, uint8_t length, uint8_t *data) override;

/**
  * @brief  Class destructor.
  * @param  none
  * @retval none
  */  
  ~SMBUS_I2C_IF() override;

private:
/**
  * @brief  File descriptor for /dev/i2c-0, used to send data over I2C.
  */
  int fd;
};

#endif
