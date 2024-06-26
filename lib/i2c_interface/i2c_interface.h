/**
  ******************************************************************************
  * @file    i2c_interface.h
  * @author  Ali Batuhan KINDAN
  * @author  Adam Englebright
  * @date    28.12.2020
  * @brief   This file constains I2C Interface class definition.
  *
  * MIT License
  *
  * Copyright (c) 2022 Ali Batuhan KINDAN
  * Cpoyright (c) 2024 Adam Englebright
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
#include <cstdint>
#include <string>
#include <stdint.h>

#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

/** I2C status types. 
 * TODO: will be extended for different I2C error types! Keep it as enum for now. */
enum i2c_status_t 
{
  I2C_STATUS_SUCCESS = 0x00,
  I2C_STATUS_ERROR = 0x01,
  I2C_STATUS_NONE = 0x02
};

/** I2C clock speed types */
enum class i2c_clockspeed_t
{
  CLK_NONE = 0,
  CLK_100KHz = 100000,
  CLK_200KHz = 200000,
  CLK_400KHz = 400000
};

/** I2C Interface class to make sensor driver work with
 * other MCU architectures by simply overriding this virtual
 * methods according to current architecture I2C driver methods. */
class I2C_Interface
{
public:
  /**
  * @brief  Class constructor.
  * @param  none
  * @retval none
  */
  I2C_Interface() {/* empty constructor */};

  /**
  * @brief  Class destructor
  * @param  none
  * @retval none
  */
  virtual ~I2C_Interface() {/* empty virtual destructor */};

  /* TODO: Init_I2C methods should be differenciated by compiler without the need of casting! */

  /**
  * @brief  I2C peripheral initialization method.
  * @param  clock I2C clock frequency (default 100 kHz)
  * @retval i2c_status_t
  */
  virtual i2c_status_t Init_I2C(i2c_clockspeed_t clock = i2c_clockspeed_t::CLK_100KHz) {return I2C_STATUS_NONE;};

  /**
  * @brief  I2C peripheral initialization method.
  * @param  slaveAddress adress of the device that will be communicated
  * @param  i2cFile 12c device file name/path
  * @retval i2c_status_t
  */
  virtual i2c_status_t Init_I2C(uint8_t slaveAddress, std::string i2cFile) {return I2C_STATUS_NONE;};

  /**
  * @brief  This method will be used for reading the data of the given register from
  * the slave with given address.
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address to be read
  * @param  status Pointer for operation status
  * @retval uint8_t Read register value
  */
  virtual uint8_t ReadRegister(uint8_t slaveAddress, uint8_t regAddress, i2c_status_t *status = nullptr) = 0;

  /**
  * @brief  This method will be used for reading the data of the given register from
  * the slave with given address. For registers that store a word of data.
  * Interprets register data as little endian (LSB stored in lower address).
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address to be read
  * @param  status Pointer for operation status
  * @retval uint16_t Read register value
  */
  virtual uint16_t ReadRegisterWordLittleEndian(uint8_t slaveAddress, uint8_t regAddress, i2c_status_t *status = nullptr) = 0;

  /**
  * @brief  This method will be used for reading the data of the given register from
  * the slave with given address. For registers that store a word of data.
  * Interprets register data as big endian (MSB stored in lower address).
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address to be read
  * @param  status Pointer for operation status
  * @retval uint16_t Read register value
  */
  virtual uint16_t ReadRegisterWordBigEndian(uint8_t slaveAddress, uint8_t regAddress, i2c_status_t *status = nullptr) = 0;
  
  /**
  * @brief  This method will be used for writing given data to the given register of the slave device 
  * with the given address.
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address that the data to be written
  * @param  data Data to be written
  * @retval i2c_status_t
  */
  virtual i2c_status_t WriteRegister(uint8_t slaveAddress, uint8_t regAddress, uint8_t data) = 0;

  /**
  * @brief  This method will be used for writing given data to the given register of the slave device 
  * with the given address. For registers that store a word of data.
  * Interprets register data as little endian (LSB stored in lower address).
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address that the data to be written
  * @param  data Data to be written
  * @retval i2c_status_t
  */
  virtual i2c_status_t WriteRegisterWordLittleEndian(uint8_t slaveAddress, uint8_t regAddress, uint16_t data) = 0;

  /**
  * @brief  This method will be used for writing given data to the given register of the slave device 
  * with the given address. For registers that store a word of data.
  * Interprets register data as big endian (MSB stored in lower address).
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address that the data to be written
  * @param  data Data to be written
  * @retval i2c_status_t
  */
  virtual i2c_status_t WriteRegisterWordBigEndian(uint8_t slaveAddress, uint8_t regAddress, uint16_t data) = 0;

  /**
  * @brief  This method will be used for reading a bit value of the given register
  * from the slace device with the given address.
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address that the data to be read
  * @param  bitMask Bit mask to be read
  * @param  status Pointer for operation status
  * @retval bool Bit value
  */
  bool ReadRegisterBit(uint8_t slaveAddress, uint8_t regAddress, uint8_t bitMask, i2c_status_t *status);

  /**
  * @brief  This method will be used for writing a bit value of the given register
  * from the slace device with the given address.
  * @param  slaveAddress Slave chip I2C bus address
  * @param  regAddress Register address that the data to be written
  * @param  bitMask Bit mask to be set/reset
  * @param  bitVal Bit value to be written
  * @retval i2c_status_t
  */
  i2c_status_t WriteRegisterBit(uint8_t slaveAddress, uint8_t regAddress, uint8_t bitMask, bool bitVal);

  /**
   * @brief  This method will be used to read a block of bytes up to 32 bytes long,
   * starting from the given register of the slave device with the given address.
   * @param  slaveAddress Slave chip I2C bus address
   * @param  regAddress Lowest address of the registers to be read from
   * @param  length Number of bytes to be read
   * @param  data Pointer to the array of bytes to be writen to
   * @retval i2c_status_t
   */
  virtual i2c_status_t ReadRegisterBlock(uint8_t slaveAddress, uint8_t regAddress, uint8_t length, uint8_t *data) = 0;

  /**
   * @brief  This method will be used to write a block of bytes up to 32 bytes long,
   * starting from the given register of the slave device with the given address.
   * @param  slaveAddress Slave chip I2C bus address
   * @param  regAddress Lowest address of the registers to be writen to
   * @param  length Number of bytes to be writen
   * @param  data Pointer to the array of bytes to be read from
   * @retval i2c_status_t
   */
  virtual i2c_status_t WriteRegisterBlock(uint8_t slaveAddress, uint8_t regAddress, uint8_t length, uint8_t *data) = 0;
};

#endif /* include guard */
