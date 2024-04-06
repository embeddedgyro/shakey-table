/**
  ******************************************************************************
  * @file    smbus_i2c_if.cpp
  * @author  Adam Englebright
  * @author  Ali Batuhan KINDAN
  * @date    28.02.2024
  * @brief   This file constains the imlementation I2C interface using the
  * kernel SMBus interface. Based on batu92k's example code for implementing
  * the I2C interface using the bcm2835 library:
  * https://github.com/batu92k/MPU6050-Sensor-Library-Cpp/blob/master/examples/RPi_4/bcm2835/bcm2835_i2c_if.cpp
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
#include "smbus_i2c_if.h"
#include <cstdint>
#include <iostream>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
extern "C" {
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
}

/**
  * @brief  I2C peripheral initialization method.
  * @param  slaveAddress adress of the device that will be communicated
  * @retval i2c_status_t
  */
i2c_status_t SMBUS_I2C_IF::Init_I2C(uint8_t slaveAddress, std::string i2cFile)
{
	/* Initalize I2C controller using device file and corresponding
	 * file descriptor. */
	fd = open(i2cFile.c_str(), O_RDWR); // Open divice file and get file descriptor
	if (fd < 0) { // Catch errors
		std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::Init_I2C(): Unable to open " << i2cFile << ". Error code " << fd << std::endl;
		exit(fd);
	}

	int status;
	status = ioctl(fd, I2C_SLAVE, slaveAddress); // Set up the peripheral slave address
	if (status < 0) { // Catch errors
		std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::Init_I2C(): Could not set up I2C bus with " << slaveAddress << " slave address. Error code " << status << std::endl;
		exit(status);
	}

	return I2C_STATUS_SUCCESS;
}

/**
  * @brief  This method will be used for reading the data of the given register from
  * the slave with given address.
  * @param  slaveAddress Slave chip I2C bus address (not used in this RTEP5 implementation, but not removed to avoid needing a large rewrite of the library)
  * @param  regAddress Register address to be read
  * @param  status Pointer for operation status
  * @retval uint8_t Read register value
  */
uint8_t SMBUS_I2C_IF::ReadRegister(uint8_t slaveAddress, uint8_t regAddress, i2c_status_t *status)
{
	int32_t data; // Data returned from i2c_smbus_read_byte_data() is a signed 32 bit integer.
	              // Negative for error, else positive with 8 bit value for byte data (i.e. 0-255).
	data = i2c_smbus_read_byte_data(fd, regAddress); // Read a byte
	if (data < 0) { // Catch errors
	  std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::ReadRegister(): Could not read a byte at register address " << unsigned(regAddress) << " of device at address " << unsigned(slaveAddress) << ". Error code " << data << std::endl;
		*status = I2C_STATUS_ERROR;
		return 0;
	}

	status && (*status = I2C_STATUS_SUCCESS);
	return (uint8_t)data;
}

/**
  * @brief  This method will be used for writing gven data to the given register of the slave device 
  * with the given address.
  * @param  slaveAddress Slave chip I2C bus address (not used in this RTEP5 implementation, but not removed to avoid needing a large rewrite of the library)
  * @param  regAddress Register address that the data to be written
  * @param  data Data to be written
  * @retval i2c_status_t
  */
i2c_status_t SMBUS_I2C_IF::WriteRegister(uint8_t slaveAddress, uint8_t regAddress, uint8_t data)
{
	int32_t errno; // Returned value from i2c_smbus_write_byte_data() is a signed 32 bit integer.
	               // Negative for error, else zero.
	errno = i2c_smbus_write_byte_data(fd, regAddress, data); // Write a byte
	if (errno < 0) { // Catch errors
	  std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::WriteRegister(): Could not write a byte to register address " << unsigned(regAddress) << " of device at address " << unsigned(slaveAddress) << ". Error code " << errno << std::endl;
		return I2C_STATUS_ERROR;
	}

	return I2C_STATUS_SUCCESS;
}

/**
 * @brief  This method will be used to read a block of bytes up to 32 bytes long,
 * starting from the given register of the slave device with the given address.
 * @param  slaveAddress Slave chip I2C bus address
 * @param  regAddress Lowest address of the registers to be read from
 * @param  length Number of bytes to be read
 * @param  data Pointer to the array of bytes to be writen to
 * @retval i2c_status_t
 */
i2c_status_t SMBUS_I2C_IF::ReadRegisterBlock(uint8_t slaveAddress, uint8_t regAddress, uint8_t length, uint8_t *data)
{
  int32_t errno; // Returned value from i2c_smbus_read_i2c_block_data() is a signed 32 bit integer.
                 // Negative if error, else the number of bytes read.
  
  // Check length is 32 max
  if (length > I2C_SMBUS_BLOCK_MAX) {
    std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::ReadRegisterBlock(): SMBus I2C block read cannot be greater than 32 bytes long." << std::endl;
    return I2C_STATUS_ERROR;
  }

  errno = i2c_smbus_read_i2c_block_data(fd, regAddress, length, data);
  if (errno < 0) { // Catch errors
    std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::ReadRegisterBlock(): Could not read block bytes from register address " << unsigned(regAddress) << " of device at address " << unsigned(slaveAddress) << ". Error code " << errno << std::endl;
    return I2C_STATUS_ERROR;
  }

  return I2C_STATUS_SUCCESS;
}

/**
 * @brief  This method will be used to write a block of bytes up to 32 bytes long,
 * starting from the given register of the slave device with the given address.
 * @param  slaveAddress Slave chip I2C bus address
 * @param  regAddress Lowest address of the registers to be writen to
 * @param  length Number of bytes to be writen
 * @param  data Pointer to the array of bytes to be read from
 * @retval i2c_status_t
 */
i2c_status_t SMBUS_I2C_IF::WriteRegisterBlock(uint8_t slaveAddress, uint8_t regAddress, uint8_t length, uint8_t *data)
{
  int32_t errno; // Returned value from i2c_smbus_read_i2c_block_data() is a signed 32 bit integer.
                 // Negative if error, else the number of bytes writen.
  
  // Check length is 32 max
  if (length > I2C_SMBUS_BLOCK_MAX) {
    std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::WriteRegisterBlock(): SMBus I2C block write cannot be greater than 32 bytes long." << std::endl;
    return I2C_STATUS_ERROR;
  }

  errno = i2c_smbus_write_i2c_block_data(fd, regAddress, length, data);
  if (errno < 0) { // Catch errors
    std::cout << "ERROR: smbus_i2c_if.cpp: SMBUS_I2C_IF::WriteRegisterBlock(): Could not write block bytes to register address " << unsigned(regAddress) << " of device at address " << unsigned(slaveAddress) << ". Error code " << errno << std::endl;
    return I2C_STATUS_ERROR;
  }

  return I2C_STATUS_SUCCESS;
}

/**
  * @brief  Class destructor.
  * @param  none
  * @retval none
  */
SMBUS_I2C_IF::~SMBUS_I2C_IF()
{
        /* Close device file. */
	close(fd);
}
