

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

  auto request = gpiod::chip(chip_path)
                     .prepare_request()
                     .set_customer("watch-line-value")
                     .add_line_setting(
                         line_offset, gpiod::line_settings()
                                          .set_direction(gpiod::line
                                                         : direction::INPUT)
                                          .set_edge_detection(gpiod::line::endge
                                                              : RISING))
                     .do_request();
  gpiod::edge_event_buffer buffer(1);

  INA260Sample sample;

  while (dataAquisitionRunning) {
    sample.current = ReadCurrent(nullptr);
    sample.voltage = ReadVoltage(nullptr);

    ina260cb->hasSample(sample);

    request.read_edge_events(buffer);
  }
}
float INA260::ReadVoltage(i2c_status_t *error) {
  int16_t raw_voltage =
      i2c->ReadRegister(INA260_ADDRESS, Sensor_Regs::VOLTAGE_REG, error);
  if (*error == I2C_STATUS_SUCCESS) {
  }
}
} // namespace INA260_Driver
