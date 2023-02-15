/**
 **************************************************
 * @file        Esp.cpp
 * @brief       File for ESP, currently empty
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

#include "Esp.h"

/**
 * @brief       Function Intializes I2S driver of the ESP32
 *
 * @param       i2s_dev_t *_i2sDev
 *              Pointer of the selected I2S driver
 *
 * @note        Function must be declared static to fit into Instruction RAM of the ESP32.
 */
void IRAM_ATTR I2SInit(i2s_dev_t *_i2sDev, volatile lldesc_s **_dmaDecs, uint8_t **_buffer)
{
    // // Allocate memory for DMA descriptor and line buffer.
    // *_buffer = (uint8_t *)heap_caps_malloc(E_INK_WIDTH / 4, MALLOC_CAP_DMA);
    // *_dmaDecs = (lldesc_s *)heap_caps_malloc(sizeof(lldesc_t), MALLOC_CAP_DMA);

    // if (_dmaDecs == NULL || _dmaDecs == NULL)
    // {
    //     Serial.println("DMA Allocation ERROR");
    //     while (true)
    //         ;
    // }

    // Clean line buffer.
    // memset(_buffer, 0, E_INK_WIDTH / 4);

    // Enable I2S peripheral and reset it.
    periph_module_enable(PERIPH_I2S1_MODULE);
    periph_module_reset(PERIPH_I2S1_MODULE);

    // Reset the FIFO Buffer in I2S module.
    _i2sDev->conf.rx_fifo_reset = 1;
    _i2sDev->conf.rx_fifo_reset = 0;
    _i2sDev->conf.tx_fifo_reset = 1;
    _i2sDev->conf.tx_fifo_reset = 0;

    // Reset I2S DMA controller.
    _i2sDev->lc_conf.in_rst = 1;
    _i2sDev->lc_conf.in_rst = 0;
    _i2sDev->lc_conf.out_rst = 1;
    _i2sDev->lc_conf.out_rst = 0;

    // Reset I2S TX and RX module.
    _i2sDev->conf.rx_reset = 1;
    _i2sDev->conf.tx_reset = 1;
    _i2sDev->conf.rx_reset = 0;
    _i2sDev->conf.tx_reset = 0;

    // Set LCD mode on I2S, setup delays on SD and WR lines.
    _i2sDev->conf2.val = 0;
    _i2sDev->conf2.lcd_en = 1;
    _i2sDev->conf2.lcd_tx_wrx2_en = 1;
    _i2sDev->conf2.lcd_tx_sdx2_en = 0;

    _i2sDev->sample_rate_conf.val = 0;
    _i2sDev->sample_rate_conf.rx_bits_mod = 8;
    _i2sDev->sample_rate_conf.tx_bits_mod = 8;
    _i2sDev->sample_rate_conf.rx_bck_div_num = 2;
    _i2sDev->sample_rate_conf.tx_bck_div_num = 2;

    // Do not use APLL, divide by 4, BCK should be ~20MHz.
    _i2sDev->clkm_conf.val = 0;
    _i2sDev->clkm_conf.clka_en = 0;
    _i2sDev->clkm_conf.clkm_div_b = 0;
    _i2sDev->clkm_conf.clkm_div_a = 1;
    _i2sDev->clkm_conf.clkm_div_num = 4;

    // FIFO buffer setup. Byte packing for FIFO: 0A0B_0B0C = 0, 0A0B_0C0D = 1, 0A00_0B00 = 3. Use dual mono single data
    _i2sDev->fifo_conf.val = 0;
    _i2sDev->fifo_conf.rx_fifo_mod_force_en = 1;
    _i2sDev->fifo_conf.tx_fifo_mod_force_en = 1;
    _i2sDev->fifo_conf.tx_fifo_mod =
        1; // byte packing 0A0B_0B0C = 0, 0A0B_0C0D = 1, 0A00_0B00 = 3. Use dual mono single data
    _i2sDev->fifo_conf.rx_data_num = 1;
    _i2sDev->fifo_conf.tx_data_num = 1;
    _i2sDev->fifo_conf.dscr_en = 1;

    // Send BCK only when needed (needs to be powered on in einkOn() function and disabled in einkOff()).
    _i2sDev->conf1.val = 0;
    _i2sDev->conf1.tx_stop_en = 0;
    _i2sDev->conf1.tx_pcm_bypass = 1;

    _i2sDev->conf_chan.val = 0;
    _i2sDev->conf_chan.tx_chan_mod = 1;
    _i2sDev->conf_chan.rx_chan_mod = 1;

    _i2sDev->conf.tx_right_first = 0; //!!invert_clk; // should be false / 0
    _i2sDev->conf.rx_right_first = 0; //!!invert_clk;

    _i2sDev->timing.val = 0;
}

/**
 * @brief       Function sedns data with I2S DMA driver.
 *
 * @param       i2s_dev_t *_i2sDev
 *              Pointer of the selected I2S driver
 *
 *              lldesc_s *_dmaDecs
 *              Pointer to the DMA descriptor.
 *
 * @note        Function must be declared static to fit into Instruction RAM of the ESP32. Also, DMA descriptor must be
 * already configured!
 */
void IRAM_ATTR sendDataI2S(i2s_dev_t *_i2sDev, volatile lldesc_s *_dmaDecs)
{
    // Stop any on-going transmission (just in case).
    _i2sDev->out_link.stop = 1;
    _i2sDev->out_link.start = 0;
    _i2sDev->conf.tx_start = 0;

    // Reset the FIFO.
    _i2sDev->conf.tx_fifo_reset = 1;
    _i2sDev->conf.tx_fifo_reset = 0;

    // Reset the I2S DMA Controller.
    _i2sDev->lc_conf.out_rst = 1;
    _i2sDev->lc_conf.out_rst = 0;

    // Reset I2S TX module.
    _i2sDev->conf.tx_reset = 1;
    _i2sDev->conf.tx_reset = 0;

    // Setup a DMA descriptor.
    _i2sDev->lc_conf.val = I2S_OUT_DATA_BURST_EN | I2S_OUTDSCR_BURST_EN;
    _i2sDev->out_link.addr = (uint32_t)(_dmaDecs)&0x000FFFFF;

    // Start sending the data
    _i2sDev->out_link.start = 1;

    // Pull SPH low -> Start pushing data into the row of EPD.
    SPH_CLEAR;

    // Set CKV to HIGH.
    CKV_SET;

    // Start sending I2S data out.
    _i2sDev->conf.tx_start = 1;

    while (!_i2sDev->int_raw.out_total_eof);

    SPH_SET;

    // Clear the interrupt flags and stop the transmission.
    _i2sDev->int_clr.val = _i2sDev->int_raw.val;
    _i2sDev->out_link.stop = 1;
    _i2sDev->out_link.start = 0;
    
}

void IRAM_ATTR setI2S1pin(uint32_t _pin, uint32_t _function, uint32_t _inv)
{
    // Check if valid pin is selected
    if (_pin > 39)
        return;

    // Fast GPIO pin to MUX (maybe there is a better way to do this?).
    const uint32_t io_mux[] = {IO_MUX_GPIO0_REG,
                               IO_MUX_GPIO1_REG,
                               IO_MUX_GPIO2_REG,
                               IO_MUX_GPIO3_REG,
                               IO_MUX_GPIO4_REG,
                               IO_MUX_GPIO5_REG,
                               IO_MUX_GPIO6_REG,
                               IO_MUX_GPIO7_REG,
                               IO_MUX_GPIO8_REG,
                               IO_MUX_GPIO9_REG,
                               IO_MUX_GPIO10_REG,
                               IO_MUX_GPIO11_REG,
                               IO_MUX_GPIO12_REG,
                               IO_MUX_GPIO13_REG,
                               IO_MUX_GPIO14_REG,
                               IO_MUX_GPIO15_REG,
                               IO_MUX_GPIO16_REG,
                               IO_MUX_GPIO17_REG,
                               IO_MUX_GPIO18_REG,
                               IO_MUX_GPIO19_REG,
                               IO_MUX_GPIO20_REG,
                               IO_MUX_GPIO21_REG,
                               IO_MUX_GPIO22_REG,
                               IO_MUX_GPIO23_REG,
                               IO_MUX_GPIO24_REG,
                               IO_MUX_GPIO25_REG,
                               IO_MUX_GPIO26_REG,
                               IO_MUX_GPIO27_REG,
                               0,
                               0,
                               0,
                               0,
                               IO_MUX_GPIO32_REG,
                               IO_MUX_GPIO33_REG,
                               IO_MUX_GPIO34_REG,
                               IO_MUX_GPIO35_REG,
                               IO_MUX_GPIO36_REG,
                               IO_MUX_GPIO37_REG,
                               IO_MUX_GPIO38_REG,
                               IO_MUX_GPIO39_REG};

    // Wrong pin selected? Return!
    if (io_mux[_pin] == -1)
        return;

    // Setup GPIO Matrix for selected pin signal
    GPIO.func_out_sel_cfg[_pin].func_sel = _function; // Set the pin function
    GPIO.func_out_sel_cfg[_pin].inv_sel = _inv;       // Does pin logic needs to be inverted?
    GPIO.func_out_sel_cfg[_pin].oen_sel = 0;          // Force output enable if bit is set

    // Registers are different for GPIOs from 0 to 32 and from 32 to 40.
    if (_pin < 32)
    {
        // Enable GPIO pin (set it as output).
        GPIO.enable_w1ts = ((uint32_t)1 << _pin);
    }
    else
    {
        // Enable GPIO pin (set it as output).
        GPIO.enable1_w1ts.data = ((uint32_t)1 << (32 - _pin));
    }

    // Set the highest drive strength.
    ESP_REG(io_mux[_pin]) = 0;
    ESP_REG(io_mux[_pin]) = ((3 << FUN_DRV_S) | (2 << MCU_SEL_S));
}