#ifndef INA260_H
#define INA260_H
#include "i2c_interface.h"
#include <thread>

namespace INA260_Driver {

#define INA260_ADDRESS 0b1000000
#define SensorConst static constexpr uint8_t

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
float CURRENT = 0.00125;
float VOLTAGE = 0.00125;
float POWER = 0.01;
}; // namespace ReadingBases
enum class Alert_Conf {
  OCL = 0b1000000000000000,
  UCL = 0b0100000000000000,
  BOL = 0b0010000000000000,
  BUL = 0b0001000000000000,
  POL = 0b0000100000000000,
  CNVR = 0b0000010000000000
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
struct INA260Sample {
  float current = 0;
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
class INA260 {
public:
  INA260(I2C_Interface *comInterface, INA260Interface *inaInterface);

  i2c_status_t InitializeSensor(Alert_Conf alert_mode = Alert_Conf::CNVR,
                                Conv_Time volt_conv_time = Conv_Time::TU140,
                                Conv_Time curr_conv_time = Conv_Time::TU140,
                                Ave_Mode averaging_mode = Ave_Mode::AV1,
                                Op_Mode operating_mode = Op_Mode::CURVOLCONT);

  void begin(void);

  void end(void);

  ~INA260() { end(); }

  i2c_status_t AveragingMode(Ave_Mode ave_setting = Ave_Mode::AV1);

  i2c_status_t CurrentConvTime(Conv_Time convert_time = Conv_Time::TU140);

  i2c_status_t VoltageConvTime(Conv_Time convert_time = Conv_Time::TU140);

  i2c_status_t OperatingMode(Op_Mode operate_mode = Op_Mode::PDTRIG);

  i2c_status_t AlertSet(Alert_Conf alert_mode = Alert_Conf::CNVR);

  float ReadCurrent(void);

  float ReadVoltage(void);

  float ReadPower(void);

private:
  /** Pointer to registered I2C interface. */
  I2C_Interface *i2c = nullptr;

  /** Pointer to registered MPU6050 interface. */
  INA260Interface *ina260cb = nullptr;

  /** Data aquisition thread. */
  std::thread dataAquisitionThread;

  /** Data aquisition flag. */
  bool dataAquisitionRunning;

  void dataAquisition(void);
};
} // namespace INA260_Driver

#endif
