/**
 **************************************************
 * @file        Esp.h
 * @brief       File for ESP, only class declaration
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#ifndef __ESP_H__
#define __ESP_H__

#include "Arduino.h"

#include "defines.h"
#include "driver/periph_ctrl.h"
#include "rom/lldesc.h"
#include "soc/i2s_reg.h"
#include "soc/i2s_struct.h"
#include "soc/rtc.h"
#include "soc/soc.h"

void IRAM_ATTR I2SInit(volatile i2s_dev_t *_i2sDev, volatile lldesc_s **_dmaDecs, uint8_t **_buffer);
void IRAM_ATTR sendDataI2S(volatile i2s_dev_t *_i2sDev, volatile lldesc_s *_dmaDecs);
void IRAM_ATTR setI2S1pin(uint32_t _pin, uint32_t _function, uint32_t _inv);

/**
 * @brief       Esp class
 */
class Esp
{
  public:

  protected:
    volatile uint8_t *_dmaLineBuffer;
    volatile lldesc_s *_dmaI2SDesc;

    // Use only I2S1 (I2S0 is not compatible with 8 bit data).
    volatile i2s_dev_t *myI2S;

  private:
};

#endif