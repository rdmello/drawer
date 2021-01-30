#include <iostream>
#include <cstdint>
#include <thread>
#include <chrono>
#include <string>

#include "vterm.h"
#include "termbox.h"

void draw_checkerboard() {
    const uint64_t width = tb_width();
    const uint64_t height = tb_height();

    // struct tb_cell cell = {'A', TB_RED, TB_BLUE};

    struct tb_cell* top_left = tb_cell_buffer();

    // Just a checkerboard.
    for (uint64_t y = 0; y < height; ++y) {
        for (uint64_t x = 0; x < width; ++x) {
            const uint64_t pos = x + (width * y);
            struct tb_cell* cell = top_left + pos;
            cell->bg = (((x + y) % 2) == 0) ? TB_RED : TB_BLUE;
        }
    }

    tb_present();
}

void draw_rectangle() {
    const uint64_t width = tb_width();
    const uint64_t height = tb_height();

    // struct tb_cell cell = {'A', TB_RED, TB_BLUE};

    struct tb_cell* top_left = tb_cell_buffer();

    // Top & bottom lines
    for (uint64_t x = 0; x < width; ++x) {
        const uint64_t bottom_pos = x + (width * (height - 1));

        struct tb_cell* cell = top_left + x;
        cell->bg = TB_RED;
        cell = top_left + bottom_pos;
        cell->bg = TB_RED;
    }

    // Left & right lines
    for (uint64_t y = 0; y < height; ++y) {
        const uint64_t left_pos = 0 + (width * y);
        const uint64_t right_pos = (width-1) + (width * y);

        struct tb_cell* cell = top_left + left_pos;
        cell->bg = TB_RED;
        cell = top_left + right_pos;
        cell->bg = TB_RED;
    }

    tb_present();
}

void draw_vterm_state(VTermScreen* screen) {
    VTermScreenCell cell;

    // Get all the vterm screen cells and update the termbox cells accordingly.
    struct tb_cell* top_left = tb_cell_buffer();
    const uint64_t width = tb_width();
    const uint64_t height = tb_height();

    for (uint64_t y = 1; y < height - 1; ++y) {
        for (uint64_t x = 1; x < width - 1; ++x) {
            vterm_screen_get_cell(screen, VTermPos{static_cast<int>(y), static_cast<int>(x)}, &cell);

            const uint64_t pos = x + (width * y);
            struct tb_cell* dest = top_left + pos;
            dest->ch = cell.chars[0];
        }
    }

    tb_present();
}

int main(int argc, char** argv) {

    std::puts("Hello, world!");
    int result = tb_init();

    if (result < 0) {
        std::puts("termbox initialization failed!!");
        return result;
    }
    std::puts(", world!");

    VTerm* term = vterm_new(tb_height() - 2, tb_width() - 2);
    vterm_set_utf8(term, true);
    VTermScreen* screen = vterm_obtain_screen(term);
    vterm_screen_enable_altscreen(screen, true);
    vterm_screen_reset(screen, 1);

    vterm_keyboard_unichar(term, 'v', VTERM_MOD_NONE);
    vterm_keyboard_unichar(term, 'i', VTERM_MOD_NONE);
    vterm_keyboard_unichar(term, 'm', VTERM_MOD_NONE);
    vterm_keyboard_key(term, VTERM_KEY_ENTER, VTERM_MOD_NONE);

    // vterm_input_write(term, "\r\n\r\n", 4);
    // vterm_screen_flush_damage(screen);

    // Wait for vim to start (??)
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // draw_rectangle();
    draw_vterm_state(screen);

    // std::this_thread::sleep_for(std::chrono::seconds(10));

    struct tb_event evt;
    tb_poll_event(&evt);

    vterm_free(term);
    
    tb_shutdown();

    // Handle the event
    switch (evt.type) {
        case TB_EVENT_KEY:
            std::cout << "Received key event: " << evt.ch << std::endl;
            break;
        case TB_EVENT_RESIZE:
            std::cout << "Received resize event: " << evt.w << " x " << evt.h << std::endl;
            break;
        case TB_EVENT_MOUSE:
            std::cout << "Received mouse event: " << evt.x << " x " << evt.y << std::endl;
            break;
    }

    return 0;
}
