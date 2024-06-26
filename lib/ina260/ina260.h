#ifndef INA260_H
#define INA260_H
#include "../i2c_interface/i2c_interface.h"
#include <gpiod.hpp>
#include <thread>

namespace INA260_Driver {

#define INA260_ADDRESS 0b1000000
#define SensorConst static constexpr uint8_t
#define ReadingBasesConst static constexpr float

namespace Sensor_Regs {
SensorConst CONF_REG = 0x00;
SensorConst CURRENT_REG = 0x01;
SensorConst VOLTAGE_REG = 0x02;
SensorConst POWER_REG = 0x03;
SensorConst MASKEN_REG = 0x06;
SensorConst ALERT_LIM = 0x07;
SensorConst MAN_ID = 0xfe;
SensorConst DIE_ID = 0xff;
}; // namespace Sensor_Regs

namespace ReadingBases {
ReadingBasesConst CURRENT = 0.00125;
ReadingBasesConst VOLTAGE = 0.00125;
ReadingBasesConst POWER = 0.01;
}; // namespace ReadingBases

enum class Alert_Conf {
  OCL = 0b10000000,
  UCL = 0b01000000,
  BOL = 0b00100000,
  BUL = 0b00010000,
  POL = 0b00001000,
  CNVR = 0b00000100
};

enum class Ave_Mode {
  AV1 = 0,
  AV4 = 1,
  AV16 = 2,
  AV64 = 3,
  AV128 = 4,
  AV256 = 5,
  AV512 = 6,
  AV1024 = 7
};

enum class Conv_Time {
  TU140 = 0,
  TU204 = 1,
  TU332 = 2,
  TU588 = 3,
  TU1100 = 4,
  TU2116 = 5,
  TU4156 = 6,
  TU8224 = 7
};

enum class Op_Mode {
  PDTRIG = 0,
  CURTRIT = 1,
  VOLTRIG = 2,
  CURVOLTRIG = 3,
  PDCONT = 4,
  CURCONT = 5,
  VOLCONT = 6,
  CURVOLCONT = 7
};

/**
 * @brief Sample from the INA260.
 */
struct INA260Sample {
  /**
   * @brief Measured current.
   */
  float current = 0;

  /**
   * @brief Measured voltage.
   */
  float voltage = 0;
};

/**
 * @brief  Callback Interface where the callback needs to be
 * implemented by the host application.
 */
class INA260Interface {
public:
  /**
   * @brief Called after a sample has arrived.
   */
  virtual void hasSample(INA260Sample &sample) = 0;
};

/**
 * @brief INA260 driver class.
 */
class INA260 {
public:
  /**
   * @brief  Class constructor. In order to make the class communicate with
   * sensor user should pass a valid I2C_Interface class instance! Also pass a
   * valid INA260Interface class instance to work on aquired data.
   * @param  comInterface I2C interface pointer
   * @param  inaInterface INA260 listener interface pointer.
   * @param  _gpioPin GPIO pin that will listen for interrupts from the INA.
   * @retval none
   */
  INA260(I2C_Interface *comInterface, INA260Interface *inaInterface,
         gpiod::line::offset _gpioPin);

  /**
   * @brief  This method wakes and configures the sensor to a specific
   * convertion time, averaging mode, operating mode and interupt type. it
   * returns whether the initialisation was successfull
   * @param  alert_mode Sets the interupt mode for the current sensor
   * @param  volt_conv_time Sets the conversion time for the voltage measurement
   * @param  curr_conv_time Sets the conversion time for the current measurement
   * @param  averaging_mode Sets the number of samples to average to produce the
   * reading.
   * @param  operating_mode Sets the operating mode of the sensor. Triggered or
   * continuous and what conversion is used for reading.
   * @retval i2c_status_t Success status
   */
  i2c_status_t InitializeSensor(Alert_Conf alert_mode = Alert_Conf::CNVR,
                                Conv_Time volt_conv_time = Conv_Time::TU140,
                                Conv_Time curr_conv_time = Conv_Time::TU140,
                                Ave_Mode averaging_mode = Ave_Mode::AV1,
                                Op_Mode operating_mode = Op_Mode::CURVOLCONT);
  /**
   * @brief  This function will begin data aquisition in a separate thread.
   * @param  None
   * @retval None
   */
  void begin(void);

  /**
   * @brief  This method stops data aquisition.
   * @param  None
   * @retval  None
   */
  void end(void);

  /**
   * @brief  Class destructor. Simple calls end() to stope data quisition
   */
  ~INA260() { end(); }

  /**
   * @brief  This method configures the number of samples that are averaged from
   * the sensor.
   * @param  ave_setting Number of samples to average
   * @retval i2c_status_t
   */
  i2c_status_t AveragingMode(Ave_Mode ave_setting = Ave_Mode::AV1);

  /**
   * @brief  This method configures the conversion time between samples for
   * current.
   * @param  convert_time Time between samples.
   * @retval i2c_status_t
   */
  i2c_status_t CurrentConvTime(Conv_Time convert_time = Conv_Time::TU140);

  /**
   * @brief  This method configures the conversion time between samples for
   * voltage.
   * @param  convert_time Time between samples.
   * @retval i2c_status_t
   */
  i2c_status_t VoltageConvTime(Conv_Time convert_time = Conv_Time::TU140);

  /**
   * @brief  This method configures the operating mode. Can be continuous or
   * triggered and can depend on voltage or current.
   * @param operate_mode Operating mode.
   * @retval i2c_status_t
   */
  i2c_status_t OperatingMode(Op_Mode operate_mode = Op_Mode::PDTRIG);

  /**
   * @brief This method configures the interupt pin mode.
   * @param  alert_mode Interupt pin mode.
   * @retval i2c_status_t
   */
  i2c_status_t AlertSet(Alert_Conf alert_mode = Alert_Conf::CNVR);

  /**
   * @brief Read the current through the sensor.
   * @param  None
   * @retval float Current.
   */
  float ReadCurrent(void);

  /**
   * @brief Read the voltage through the sensor.
   * @param  None
   * @retval float Voltage.
   */
  float ReadVoltage(void);

  /**
   * @brief Read the power through the sensor.
   * @param  None
   * @retval float Power
   */
  float ReadPower(void);

private:
  /** Pointer to registered I2C interface. */
  I2C_Interface *i2c = nullptr;

  /** Pointer to registered MPU6050 interface. */
  INA260Interface *ina260cb = nullptr;

  /** GPIO pin that will be used to listen for interrupts from the INA */
  gpiod::line::offset gpioPin;

  /** Data aquisition thread. */
  std::thread dataAquisitionThread;

  /** Data aquisition flag. */
  bool dataAquisitionRunning;

  /**
   * @brief Data aquisition method that, in the loop, will block until and
   * interupt is generated by the INA260
   * @param None
   * @retval None
   */
  void dataAquisition(void);
};
} // namespace INA260_Driver

#endif
