#include "Screen.h"

using namespace daisy;
using namespace daisysp;


const FontDef Screen::FONT = Font_6x8;
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

    display->DrawRect(rect.GetX(), rect.GetY(), rect.GetX() + rect.GetWidth(), rect.GetY() + rect.GetHeight(), on, fill);
}

void Screen::DrawRectFilled(Rectangle rect, bool border, bool fill) {
    if (!screenOn) { return; }

    DrawRect(rect, fill, true);
    DrawRect(rect, border, false);
}

void Screen::DrawButton(Rectangle rect, std::string str, bool border, bool fill, bool text) {
    if (!screenOn) { return; }

    DrawRectFilled(rect, border, fill);
    display->WriteStringAligned(str.c_str(), FONT, rect, Alignment::centered, text);
    // display->SetCursor(rect.GetX() + 2, rect.GetY() + 2);
    // display->WriteString(str.c_str(), FONT, text);
}

void Screen::DrawMenu(uint8_t selected) {
    if (!screenOn) { return; }

    uint8_t itemWidth = FONT.FontWidth * 2 + 3;
    uint8_t itemHeight = FONT.FontWidth + 4;
    uint8_t displayCount = std::min((u8)(WIDTH / itemWidth), MENU_SIZE);
    uint8_t highlightItem = displayCount / 2;
    uint8_t start = std::min(std::max(0, selected - highlightItem), MENU_SIZE - displayCount);

    // item = menu item shown; pos = position on screen
    u8 pos = 0;
    for (uint8_t item = start; item < start + displayCount; item++) {
        if (menuItems[item].length() > 0) {
            bool sel = item == selected;
            uint8_t x = itemWidth * pos;
            uint8_t y = HEIGHT - itemHeight;
            Rectangle rect(x, y, itemWidth, itemHeight);
            DrawButton(rect, this->menuItems[item], true, sel, !sel);
            // if (sel) {  // only draw selected
            //     DrawButton(rect, this->menuItems[item], true, false, true);
            // }
            pos++;
        }
        // bool sel = item == selected;
        // uint8_t x = itemWidth * (item - start);
        // uint8_t y = HEIGHT - itemHeight;
        // Rectangle rect(x, y, itemWidth, itemHeight);
        // DrawButton(rect, this->menuItems[item], true, sel, !sel);
    }

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

void Screen::ScreensaveEvent(u8 drum) {
    if (screenOn) { return; }

    // show notes with horizontal wipe
    u8 x = sweepX % (WIDTH + 1);
    if (x > 5 && x < WIDTH + 1) {
        display->DrawCircle(x - 4, (15-drum) * 4 + 1, 1, true);
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

    char* mstr = &message[0];
    display->SetCursor(0, row * 10);
    display->WriteString(mstr, Font_6x8, true);
    display->Update();
}

void Screen::OledMessage(std::string message, int row, int column) {
    if (!screenOn) { return; }

    char* mstr = &message[0];
    display->SetCursor(column * 8, row * 10);
    display->WriteString(mstr, Font_6x8, true);
    display->Update();
}

void Screen::ShowCpu(float usage) {
    if (!screenOn) { return; }

    u8 menuHeight = FONT.FontWidth + 4;
    u8 scaled = (int)(usage * WIDTH);
    u8 qtr = WIDTH / 4;
    display->DrawRect(0, HEIGHT - menuHeight - 3, WIDTH, HEIGHT - menuHeight - 1, false, true);
    display->DrawRect(0, HEIGHT - menuHeight - 3, scaled, HEIGHT - menuHeight - 1, true, true);
    display->DrawLine(qtr, HEIGHT - menuHeight - 4, qtr, HEIGHT - menuHeight -1, true);
    display->DrawLine(2 * qtr, HEIGHT - menuHeight - 4, 2 * qtr, HEIGHT - menuHeight -1, true);
    display->DrawLine(3 * qtr, HEIGHT - menuHeight - 4, 3 * qtr, HEIGHT - menuHeight -1, true);
}
