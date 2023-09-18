/**
 **************************************************
 * @file        Buzzer.h
 * @brief       Basic funtions for controling Inkplate's buzzer
 *
 *              Only available on Inkplate PLUS2
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
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
#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef ARDUINO_INKPLATEPLUS2

#include "Arduino.h"
#include "Pcal.h"
#include "Wire.h"
#include "defines.h"

// Include digipot library
#include "libs/MCP4018/src/MCP4018-SOLDERED.h"

#define BEEP_FREQ_MAX 2933
#define BEEP_FREQ_MIN 572

class Buzzer : virtual public Expander
{
  public:
    Buzzer(){};
    void begin();
    void beep(uint32_t length, int freq);
    void beep(uint32_t length);
    void beepOn(int freq);
    void beepOn();
    void beepOff();

  private:
    MCP4018_SOLDERED digipot;
    void setFrequencyInternal(int freq);
    int freqToWiperPercent(int freq);
};

#endif

#endif