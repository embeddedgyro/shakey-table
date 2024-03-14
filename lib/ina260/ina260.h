#ifndef INA260_H
#define INA260_H
#include "i2c_interface.h"
#include <thread>

namespace INA260_Driver {
  enum class Alert_Conf{
    OCL = 0b1000000000000000,
    UCL = 0b0100000000000000,
    BOL = 0b0010000000000000,
    BUL = 0b0001000000000000
    POL = 0b0000100000000000,
    CNVR = 0b0000010000000000
  };
  enum class Ave_Mode{
    AV1 = 0,
    AV4 = 1,
    AV16 = 2,
    AV64 = 3,
    AV128 = 4,
    AV256 = 5,
    AV512 = 6,
    AV1024 = 7
  };
  
  enum class Conv_Time{
    TU140 = 0,
    TU204 = 1,
    TU332 = 2,
    TU588 = 3,
    TU1100 = 4,
    TU2116 = 5,
    TU4156 = 6,
    TU8224 = 7
  };
  
  enum class Op_Mode{
    PDTRIG = 0,
    CURTRIT = 1,
    VOLTRIG = 2,
    CURVOLTRIG = 3,
    PDCONT = 4,
    CURCONT = 5,
    VOLCONT = 6,
    CURVOLTRIG = 7
  };
  struct INA260Sample {
    float current = 0;
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
    virual void hasSample(INA260Sample& sample) = 0;

  };
  class INA260
  {
  public:
    INA260(I2C_Interface* comInterface, INA260Interface* inaInterface);
    
    i2c_status_t InitializeSensor(Alert_conf init_setting = Alert_conf::CNVR);
    
    i2c_Status_t AveragingMode(Ave_Mode ave_setting = Ave_Mode::AV1);
    
    i2c_Status_t CurrentConvTime(Conv_Time convert_time = Conv_Time::TU140);

    i2c_Status_t VoltageConvTime(Conv_Time convert_time = Conv_Time::TU140);
    
    i2c_Status_t OperatingMode(Op_Mode operate_mode = Op_Mode::PDTRIG);

  }
}

#endif
