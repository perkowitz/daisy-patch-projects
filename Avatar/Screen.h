#ifndef SCREEN_H
#define SCREEN_H

#include "daisy_patch.h"
#include "daisysp.h"
#include <string>
#include "lib/Utility.h"

using namespace daisy;
using namespace daisysp;


class Screen {

    public:
        static const u8 HEIGHT = 63;
        static const u8 WIDTH = 127;
        static const FontDef FONT;
        static const FontDef MENU_FONT;
        static const FontDef TITLE_FONT;
        static const u8 MENU_SIZE = 6;
        static const u16 SCREEN_SCAN_TIME = 2000 / WIDTH;
        static std::string menuItems[MENU_SIZE];

        Screen() {
            this->display = nullptr;
        }

        // Screen(OledDisplay<SSD130x4WireSpi128x64Driver> *display) {
        //     this->display = display;
        // }

        void setDisplay(OledDisplay<SSD130x4WireSpi128x64Driver> *display) {
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
        void DrawButton(Rectangle rect, std::string str, bool border, bool text, bool fill, Alignment alignment);
        void DrawPageTitle(std::string moduleName, std::string pageTitle);

        void SetScreenOn(bool screenOn);
        bool IsScreenOn() { return screenOn; }
        void Screensave(u32 time);
        void ScreensaveEvent(u8 note, bool on);
        // void DrawHachiLogo(u8 x);

        void OledMessage(std::string message, int row);
        void OledMessage(std::string message, int row, int column);
        void Write(std::string str, u8 x, u8 y);

        void ShowCpu(float usage, bool showGraphic);

    private:
        OledDisplay<SSD130x4WireSpi128x64Driver> *display;
        bool screenOn = true;
        u8 sweepX = 0;
        u32 lastScreenSaveUpdate = 0;
        const static u8 screenSweepRate = 6;

};


#endif
