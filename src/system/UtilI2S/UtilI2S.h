/**
 **************************************************
 * @file        Esp.h
 * @brief       File for ESP, only class declaration
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

#ifndef __UTILI2S_H__
#define __UTILI2S_H__

#include "Arduino.h"
#include "driver/periph_ctrl.h"
#include "rom/lldesc.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"
#include "soc/i2s_reg.h"
#include "soc/i2s_struct.h"
#include "soc/rtc.h"
#include "soc/soc.h"

/**
 * @brief       I2S class used to send data to the panel.
 */
class UtilI2S
{
  public:
    void IRAM_ATTR I2SInit(volatile i2s_dev_t *_i2sDev, uint8_t _clockDivider = 5);
    void IRAM_ATTR sendDataI2S(volatile i2s_dev_t *_i2sDev, volatile lldesc_s *_dmaDecs);
    void IRAM_ATTR setI2S1pin(uint32_t _pin, uint32_t _function, uint32_t _inv);

  protected:
    volatile uint8_t *_dmaLineBuffer;
    volatile lldesc_s *_dmaI2SDesc;

    // Use only I2S1 (I2S0 is not compatible with 8 bit data).
    volatile i2s_dev_t *myI2S;

  private:
};

#endif