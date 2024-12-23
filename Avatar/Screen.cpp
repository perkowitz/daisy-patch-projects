#include "Screen.h"

using namespace daisy;
using namespace daisysp;


const FontDef Screen::FONT = Font_6x8;
const FontDef Screen::MENU_FONT = Font_7x10;
const FontDef Screen::TITLE_FONT = Font_11x18;
const FontDef font1 = Font_11x18;
const FontDef font2 = Font_16x26;
const FontDef font3 = Font_7x10;
std::string Screen::menuItems[Screen::MENU_SIZE];


void Screen::DrawRect(uint_fast8_t x1, uint_fast8_t y1, uint_fast8_t x2, uint_fast8_t y2, bool on, bool fill) {
    if (!screenOn) { return; }

    display->DrawRect(x1, y1, x2, y2, on, fill);
}

Rectangle Screen::WriteStringAligned(
        const char*    str,
        const FontDef& font,
        Rectangle      boundingBox,
        Alignment      alignment,
        bool           on) {

    if (!screenOn) { return boundingBox; }

    return display->WriteStringAligned(str, font, boundingBox, alignment, on);
}

void Screen::DrawLine(
    uint_fast8_t x1,
    uint_fast8_t y1,
    uint_fast8_t x2,
    uint_fast8_t y2,
    bool         on) {

        if (!screenOn) { return; }

        display->DrawLine(x1, y1, x2, y2, on);
}

void Screen::DrawRect(Rectangle rect, bool on, bool fill) {
    if (!screenOn) { return; }

    if (rect.GetX() >= 0 && rect.GetX() < WIDTH &&
            rect.GetY() >= 0 && rect.GetY() < HEIGHT &&
            rect.GetRight() >= 0 && rect.GetRight() < WIDTH &&
            rect.GetBottom() >= 0 && rect.GetBottom() < HEIGHT) {
        display->DrawRect(rect.GetX(), rect.GetY(), rect.GetX() + rect.GetWidth(), rect.GetY() + rect.GetHeight(), on, fill);
    }
}

void Screen::DrawRectFilled(Rectangle rect, bool border, bool fill) {
    if (!screenOn) { return; }

    DrawRect(rect, fill, true);
    DrawRect(rect, border, false);
}

void Screen::DrawButton(Rectangle rect, std::string str, bool border, bool fill, bool text) {
    DrawButton(rect, str, border, fill, text, Alignment::centered);
}

void Screen::DrawButton(Rectangle rect, std::string str, bool border, bool fill, bool text, Alignment alignment) {
    if (!screenOn) { return; }

    DrawRectFilled(rect, border, fill);
    if (rect.GetX() >= 0 && rect.GetX() <= WIDTH &&
            rect.GetY() >= 0 && rect.GetY() <= HEIGHT &&
            rect.GetRight() >= 0 && rect.GetRight() <= WIDTH &&
            rect.GetBottom() >= 0 && rect.GetBottom() <= HEIGHT) {
        display->WriteStringAligned(str.c_str(), FONT, rect, alignment, text);
    }
}

void Screen::DrawPageTitle(std::string moduleName, std::string pageTitle) {
    if (!screenOn) { return; }

    Rectangle titleRect(0, HEIGHT - 20, WIDTH, 20);

    display->WriteStringAligned(pageTitle.c_str(), MENU_FONT, titleRect, Alignment::bottomLeft, true);
    display->WriteStringAligned(moduleName.c_str(), TITLE_FONT, titleRect, Alignment::bottomRight, true);
}

void Screen::SetScreenOn(bool screenOn) { 
    this->screenOn = screenOn; 
    if (!screenOn) {
        sweepX = 0;
        lastScreenSaveUpdate = 0;
    }
}

void Screen::Screensave(u32 time) {
    if (screenOn) { return; }

    if (time - lastScreenSaveUpdate >= SCREEN_SCAN_TIME) {
        u8 x = sweepX % (WIDTH + 1);
        display->DrawLine(x, 0, x, HEIGHT, false);
        if (x < WIDTH - 1) {
            display->DrawLine(x + 1, 0, x + 1, HEIGHT, true);
        }

        display->Update();
        sweepX++;
        lastScreenSaveUpdate = time;
    }
}

void Screen::ScreensaveEvent(u8 note, bool on) {
    if (screenOn) { return; }

    // show notes with horizontal wipe
    if (on) {
        note = Utility::LimitInt(note, 0, HEIGHT);
        u8 x = sweepX % (WIDTH + 1);
        if (x > 5 && x < WIDTH + 1) {
            display->DrawPixel(x - 4, note, true);
        }
    }
}

// void Screen::DrawHachiLogo(u8 startX) {
//     u8 w = 8;
//     u8 w2 = 5;

//     for (u8 row = 0; row < 8; row++) {
//         if (row != 2 && row != 6) {
//             for (u8 i = 0; i < 2; i++) {
//                 u8 x = startX + i * w;
//                 Rectangle r(x, row * w, w2, w2);
//                 display->DrawRect(r.GetX(), r.GetY(), r.GetRight(), r.GetBottom(), true, false);
//                 // display->DrawPixel(r.GetX(), r.GetY(), false);
//                 // display->DrawPixel(r.GetX(), r.GetBottom(), false);
//                 // display->DrawPixel(r.GetRight(), r.GetY(), false);
//                 // display->DrawPixel(r.GetRight(), r.GetBottom(), false);
//             }
//         }
//     }
// }

void Screen::OledMessage(std::string message, int row) {
    if (!screenOn) { return; }

    Write(message, 0, row * 10);
}

void Screen::OledMessage(std::string message, int row, int column) {
    if (!screenOn) { return; }

    Write(message, column * 8, row * 10);
}

void Screen::Write(std::string message, u8 x, u8 y) {
    if (!screenOn) { return; }

    Rectangle rect(x, y, WIDTH - x, HEIGHT - y);
    DrawButton(rect, message, false, false, true, Alignment::topLeft);
    display->Update();
}


void Screen::ShowCpu(float usage, bool showGraphic) {
    if (!screenOn) { return; }

    std::string  cpu = "cpu:" + std::to_string((int)(usage * 100)) + "%";
    Write(cpu, 40, 0);
    Write(cpu, 40, 80);

    if (showGraphic) {
        u8 menuHeight = FONT.FontWidth + 4;
        u8 scaled = (int)(usage * WIDTH);
        u8 qtr = WIDTH / 4;
        display->DrawRect(0, HEIGHT - menuHeight - 3, WIDTH, HEIGHT - menuHeight - 1, false, true);
        display->DrawRect(0, HEIGHT - menuHeight - 3, scaled, HEIGHT - menuHeight - 1, true, true);
        display->DrawLine(qtr, HEIGHT - menuHeight - 4, qtr, HEIGHT - menuHeight -1, true);
        display->DrawLine(2 * qtr, HEIGHT - menuHeight - 4, 2 * qtr, HEIGHT - menuHeight -1, true);
        display->DrawLine(3 * qtr, HEIGHT - menuHeight - 4, 3 * qtr, HEIGHT - menuHeight -1, true);
    }
}
