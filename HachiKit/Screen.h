#ifndef SCREEN_H
#define SCREEN_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "Utility.h"

using namespace daisy;
using namespace daisysp;

class Screen {

    public:
        static const u8 HEIGHT = 63;
        static const u8 WIDTH = 127;
        static const FontDef FONT;
        static const u8 MENU_SIZE = 20;
        static std::string menuItems[MENU_SIZE];

        Screen(OledDisplay<SSD130x4WireSpi128x64Driver> *display) {
            this->display = display;
        }

        // pass-throughs to hw display
        void DrawRect(uint_fast8_t x1, uint_fast8_t y1, uint_fast8_t x2, uint_fast8_t y2, bool on, bool fill);
        Rectangle WriteStringAligned(const char*    str,
                                 const FontDef& font,
                                 Rectangle      boundingBox,
                                 Alignment      alignment,
                                 bool           on);
        void DrawLine(uint_fast8_t x1,
                    uint_fast8_t y1,
                    uint_fast8_t x2,
                    uint_fast8_t y2,
                    bool         on);


        void DrawRect(Rectangle rect, bool on, bool fill);
        void DrawRectFilled(Rectangle rect, bool border, bool fill);

        void DrawButton(Rectangle rect, std::string str, bool border, bool text, bool fill);

        void DrawMenu(uint8_t selected);

        void SetScreenOn(bool screenOn);
        bool IsScreenOn() { return screenOn; }
        void Screensave();
        void ScreensaveEvent(u8 drum);
        // void DrawHachiLogo(u8 x);

        void OledMessage(std::string message, int row);
        void OledMessage(std::string message, int row, int column);

        void ShowCpu(float usage);

    private:
        OledDisplay<SSD130x4WireSpi128x64Driver> *display;
        bool screenOn = true;
        u16 screenCounter = 0;
        const static u8 screenSweepRate = 6;

};


#endif
