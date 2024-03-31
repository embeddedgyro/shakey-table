

#include "ina260.h"
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
  const std::filesystem::path chip_path("/dev/gpiochip0");
  const gpiod::line::offset line_offset = 5;

  auto request =
      gpiod::chip(chip_path)
          .prepare_request()
          .set_consumer("watch-line-value")
          .add_line_settings(
              line_offset, gpiod::line_settings()
                               .set_direction(gpiod::line ::direction::INPUT)
                               .set_edge_detection(gpiod::line::edge ::RISING))
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
  uint8_t voltage_data[2];
  i2c->ReadRegisterBlock(INA260_ADDRESS, Sensor_Regs::VOLTAGE_REG, 2,
                         (uint8_t *)voltage_data);
  signed int int_voltage = voltage_data[1] | (voltage_data[0] << 8);
  if (int_voltage & 0x8000) {
    int_voltage = int_voltage - 0x10000;
  }
  return ReadingBases::VOLTAGE * int_voltage;
}

float INA260::ReadCurrent(void) {
  uint8_t current_data[2];
  i2c->ReadRegisterBlock(INA260_ADDRESS, Sensor_Regs::CURRENT_REG, 2,
                         (uint8_t *)current_data);
  signed int int_current = current_data[1] | (current_data[0] << 8);
  if (int_current & 0x8000) {
    int_current = int_current - 0x10000;
  }
  return ReadingBases::CURRENT * int_current;
}

float INA260::ReadPower(void) {
  uint8_t Power_data[2];
  i2c->ReadRegisterBlock(INA260_ADDRESS, Sensor_Regs::POWER_REG, 2,
                         (uint8_t *)Power_data);
  signed int int_power = Power_data[1] | (Power_data[0] << 8);
  if (int_power & 0x8000) {
    int_power = int_power - 0x10000;
  }
  return ReadingBases::POWER * int_power;
}

i2c_status_t INA260::AlertSet(Alert_Conf alert_mode) {
  uint8_t alert_data[2];
  alert_data[1] = (uint8_t)alert_mode;
  alert_data[0] = 0x00;
  return i2c->WriteRegisterBlock(INA260_ADDRESS, Sensor_Regs::MASKEN_REG, 2,
                                 (uint8_t *)alert_data);
}
} // namespace INA260_Driver
