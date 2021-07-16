#include "EdgeDisplay.hpp"

EdgeDisplay::EdgeDisplay(EdgeControl_LiquidCrystalClass *lcd)
    : _lcd(lcd)
{

}

void EdgeDisplay::begin(uint8_t cols, uint8_t rows)
{
    _lcd->begin(cols, rows);
    _lcd->leftToRight();
    _lcd->backlight();
    _lcd->noCursor();
}

void EdgeDisplay::reset()
{
    _lcd->clear();
}

void EdgeDisplay::showMessage(const String& msg)
{
    reset();

    _lcd->print(msg);
}

void EdgeDisplay::showError(const String& err)
{
    reset();

    _lcd->print("ERR encountered:");
    _lcd->setCursor(0, 1);
    _lcd->print(err);
}

void EdgeDisplay::showTelemetry(const String& telem)
{
    reset();

    _lcd->setCursor(16, 0);
    _lcd->print(telem);

    for(uint8_t i = 0; i < 16; i++)
        _lcd->scrollDisplayLeft();
        
    showMessage("OK");
}
