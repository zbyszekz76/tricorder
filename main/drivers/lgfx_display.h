#pragma once

#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32/Bus_SPI.hpp>

// =================================================
// ===================== LGFX ======================
// =================================================

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel;
    lgfx::Bus_SPI _bus;

public:

    LGFX(void)
    {
        // ================= BUS =================

        {
            auto cfg = _bus.config();

            cfg.spi_host = SPI2_HOST;

            cfg.spi_mode = 0;

            cfg.freq_write = 20000000;
            cfg.freq_read  = 10000000;

            cfg.spi_3wire = false;

            cfg.use_lock = true;

            cfg.dma_channel = SPI_DMA_CH_AUTO;

            cfg.pin_sclk = 12;
            cfg.pin_mosi = 11;
            cfg.pin_miso = 13;

            cfg.pin_dc = 14;

            _bus.config(cfg);

            _panel.setBus(&_bus);
        }

        // ================= PANEL =================

        {
            auto cfg = _panel.config();

            cfg.pin_cs = 10;
            cfg.pin_rst = 15;

            cfg.panel_width = 240;
            cfg.panel_height = 320;

            cfg.offset_x = 0;
            cfg.offset_y = 0;

            cfg.offset_rotation = 0;

            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;

            cfg.readable = true;

            cfg.invert = false;
            cfg.rgb_order = false;

            cfg.dlen_16bit = false;

            cfg.bus_shared = true;

            _panel.config(cfg);
        }

        setPanel(&_panel);
    }
};

// =================================================
// ===================== GLOBAL ====================
// =================================================

extern LGFX tft;
