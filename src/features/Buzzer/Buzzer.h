/**
 **************************************************
 * @file        Buzzer.h
 * @brief       Basic funtions for controling Inkplate's buzzer
 *
 *              Only available on Inkplate 4TEMPERA
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
#ifndef __BUZZER_H__
#define __BUZZER_H__

#ifdef ARDUINO_INKPLATE4TEMPERA

#include "Arduino.h"

// Include digipot library
#include "../MCP4018/MCP4018-SOLDERED.h"

#define BEEP_FREQ_MAX 2933
#define BEEP_FREQ_MIN 572

class Inkplate;

class Buzzer
{
  public:
    Buzzer(){};
    void begin(Inkplate *_inkplatePtr);
    void init();
    void beep(uint32_t length, int freq);
    void beep(uint32_t length);
    void beepOn(int freq);
    void beepOn();
    void beepOff();

  private:
    MCP4018_SOLDERED digipot;
    Inkplate *_inkplate = NULL;
    void setFrequencyInternal(int freq);
    int freqToWiperPercent(int freq);
};

#endif

#endif