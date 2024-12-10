#include "Screen.h"

using namespace daisy;
using namespace daisysp;


const FontDef Screen::FONT = Font_6x8;
const FontDef Screen::MENU_FONT = Font_7x10;
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
    DrawSimpleMenu(selected);
}

void Screen::DrawSimpleMenu(uint8_t selected) {
    if (!screenOn) { return; }

    display->SetCursor(2, HEIGHT - 20);
    display->WriteString(menuItems[selected].c_str(), MENU_FONT, true);
}

void Screen::DrawPageTitle(std::string moduleName, std::string pageTitle) {
    if (!screenOn) { return; }

    display->SetCursor(2, HEIGHT - 20);
    display->WriteString((moduleName + ":" + pageTitle).c_str(), MENU_FONT, true);
}


// void Screen::DrawLinearMenu(uint8_t selected) {
//     if (!screenOn) { return; }

//     uint8_t itemWidth = FONT.FontWidth * 2 + 3;
//     uint8_t itemHeight = FONT.FontWidth + 4;
//     uint8_t displayCount = std::min((u8)(WIDTH / itemWidth), MENU_SIZE);
//     uint8_t highlightItem = displayCount / 2;
//     uint8_t start = std::min(std::max(0, selected - highlightItem), MENU_SIZE - displayCount);

//     // item = menu item shown; pos = position on screen
//     u8 pos = 0;
//     for (uint8_t item = start; item < start + displayCount; item++) {
//         if (menuItems[item].length() > 0) {
//             bool sel = item == selected;
//             uint8_t x = itemWidth * pos;
//             uint8_t y = HEIGHT - itemHeight;
//             Rectangle rect(x, y, itemWidth, itemHeight);
//             // DrawButton(rect, this->menuItems[item], true, sel, !sel);
//             if (sel) {  // only draw selected
//                 DrawButton(rect, this->menuItems[item], true, false, true);
//             }
//             pos++;
//         }
//         // bool sel = item == selected;
//         // uint8_t x = itemWidth * (item - start);
//         // uint8_t y = HEIGHT - itemHeight;
//         // Rectangle rect(x, y, itemWidth, itemHeight);
//         // DrawButton(rect, this->menuItems[item], true, sel, !sel);
//     }

// }

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

    // u8 bx = (sweepX + 17);
    // if (bx < 200 && bx > 20) {
    //     display->Fill(false);
    //     // display->DrawCircle(bx, 40, 12, false);
    //     // display->DrawCircle(bx + 10, 30, 9, false);
    //     display->DrawCircle(bx, 40, 10, true);
    //     display->DrawLine(bx - 16, 40, bx - 10, 40, true);
    //     // display->DrawLine(bx - 10, 40, x - 16, 37, true);
    //     // display->DrawLine(bx - 10, 40, x - 16, 43, true);
    //     display->DrawLine(bx - 1, 50, bx + 1, 53, true);
    //     display->DrawLine(bx + 1, 50, bx + 1, 53, true);
    //     display->DrawCircle(bx + 10, 30, 7, true);
    //     display->DrawLine(bx + 17, 30, bx + 21, 30, true);
    //     display->DrawCircle(bx + 12, 28, 1, true);
    // }
}

void Screen::ScreensaveEvent(u8 note) {
    if (screenOn) { return; }

    // show notes with horizontal wipe
    note = Utility::LimitInt(note, 0, HEIGHT);
    u8 x = sweepX % (WIDTH + 1);
    if (x > 5 && x < WIDTH + 1) {
        display->DrawPixel(x - 4, note, true);
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
