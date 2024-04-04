

#include "ina260.h"
#include <cstdint>
#include <gpiod.hpp>

namespace INA260_Driver {
INA260::INA260(I2C_Interface *comInterface, INA260Interface *inaInterface) {
  if (comInterface)
    this->i2c = comInterface;
  if (inaInterface)
    this->ina260cb = inaInterface;
}

i2c_status_t INA260::InitializeSensor(Alert_Conf alert_mode,
                                      Conv_Time volt_conv_time,
                                      Conv_Time curr_conv_time,
                                      Ave_Mode averaging_mode,
                                      Op_Mode operating_mode) {
  i2c_status_t result = AlertSet(alert_mode);

  if (result == I2C_STATUS_SUCCESS)
    result = CurrentConvTime(curr_conv_time);

  if (result == I2C_STATUS_SUCCESS)
    result = VoltageConvTime(volt_conv_time);

  if (result == I2C_STATUS_SUCCESS)
    result = AveragingMode(averaging_mode);

  if (result == I2C_STATUS_SUCCESS)
    result = OperatingMode(operating_mode);
  return result;
}

void INA260::begin(void) {
  dataAquisitionRunning = true;
  dataAquisitionThread = std::thread(&INA260::dataAquisition, this);
}

void INA260::end(void) {
  dataAquisitionRunning = false;
  dataAquisitionThread.join();
}

void INA260::dataAquisition(void) {
  const std::filesystem::path chip_path("/dev/gpiochip4");
  const gpiod::line::offset line_offset = 17;

  auto request =
      gpiod::chip(chip_path)
          .prepare_request()
          .set_consumer("watch-line-value")
          .add_line_settings(
              line_offset, gpiod::line_settings()
                               .set_direction(gpiod::line ::direction::INPUT)
                               .set_edge_detection(gpiod::line::edge ::FALLING))
          .do_request();
  gpiod::edge_event_buffer buffer(1);

  INA260Sample sample;

  while (dataAquisitionRunning) {
    sample.current = ReadCurrent();
    sample.voltage = ReadVoltage();

    ina260cb->hasSample(sample);

    request.read_edge_events(buffer);
  }
}

float INA260::ReadVoltage(void) {
  uint16_t voltage_data =
      i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::VOLTAGE_REG);
  i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::MASKEN_REG);
  return ReadingBases::VOLTAGE * voltage_data;
}

float INA260::ReadCurrent(void) {
  uint16_t current_data =
      i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::VOLTAGE_REG);
  i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::MASKEN_REG);
  return ReadingBases::CURRENT * current_data;
}

float INA260::ReadPower(void) {
  uint16_t power_data =
      i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::VOLTAGE_REG);
  i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::MASKEN_REG);
  return ReadingBases::POWER * power_data;
}

i2c_status_t INA260::AlertSet(Alert_Conf alert_mode) {
  uint16_t alert_data;
  alert_data = (uint8_t)alert_mode >> 8;
  return i2c->WriteRegisterWord(INA260_ADDRESS, Sensor_Regs::MASKEN_REG,
                                alert_data);
}

i2c_status_t INA260::CurrentConvTime(Conv_Time convert_time) {
  uint16_t conv_time_reg =
      i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG);
  uint16_t mask = ~0b0000000000111000;
  conv_time_reg = conv_time_reg & mask;
  conv_time_reg = conv_time_reg | ((uint16_t)convert_time << 3);
  return i2c->WriteRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG,
                                conv_time_reg);
}

i2c_status_t INA260::VoltageConvTime(Conv_Time convert_time) {
  uint16_t conv_time_reg =
      i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG);
  uint16_t mask = ~0b0000000111000000;
  conv_time_reg = conv_time_reg & mask;
  conv_time_reg = conv_time_reg | ((uint16_t)convert_time << 6);
  return i2c->WriteRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG,
                                conv_time_reg);
}

i2c_status_t INA260::OperatingMode(Op_Mode operate_mode) {
  uint16_t conf_reg_data =
      i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG);
  uint16_t mask = ~0b0000000000000111;
  conf_reg_data = conf_reg_data & mask;
  conf_reg_data = conf_reg_data | ((uint16_t)operate_mode);
  std::cout << "Data sent to config register: " << std::hex << conf_reg_data << std::endl;
  return i2c->WriteRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG,
                                conf_reg_data);
}

i2c_status_t INA260::AveragingMode(Ave_Mode ave_setting) {
  uint16_t conf_reg_data =
      i2c->ReadRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG);
  uint16_t mask = ~0b0000111000000000;
  conf_reg_data = conf_reg_data & mask;
  conf_reg_data = conf_reg_data | ((uint16_t)ave_setting << 9);
  return i2c->WriteRegisterWord(INA260_ADDRESS, Sensor_Regs::CONF_REG,
                                conf_reg_data);
}

} // namespace INA260_Driver
