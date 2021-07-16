#pragma once

#include <Arduino_EdgeControl.h>

class EdgeDisplay
{
    public:

        EdgeDisplay(EdgeControl_LiquidCrystalClass *lcd);

        void
            begin(uint8_t cols, uint8_t rows),
            reset(),
            showMessage(const String& msg),
            showError(const String& err),
            showTelemetry(const String& telem);

    private:

        EdgeControl_LiquidCrystalClass *_lcd;

};
