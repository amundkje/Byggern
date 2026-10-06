#include "oled.h"
#include "spi.h"
#include <avr/pgmspace.h>

// Five-column font for ASCII space through Z. Lowercase uses uppercase.
static const uint8_t font[][5] PROGMEM = {
    {0,0,0,0,0}, {0,0,0x5f,0,0}, {0,7,0,7,0},
    {0x14,0x7f,0x14,0x7f,0x14}, {0x24,0x2a,0x7f,0x2a,0x12},
    {0x23,0x13,8,0x64,0x62}, {0x36,0x49,0x55,0x22,0x50},
    {0,5,3,0,0}, {0,0x1c,0x22,0x41,0}, {0,0x41,0x22,0x1c,0},
    {0x14,8,0x3e,8,0x14}, {8,8,0x3e,8,8}, {0,0x50,0x30,0,0},
    {8,8,8,8,8}, {0,0x60,0x60,0,0}, {0x20,0x10,8,4,2},
    {0x3e,0x51,0x49,0x45,0x3e}, {0,0x42,0x7f,0x40,0},
    {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4b,0x31},
    {0x18,0x14,0x12,0x7f,0x10}, {0x27,0x45,0x45,0x45,0x39},
    {0x3c,0x4a,0x49,0x49,0x30}, {1,0x71,9,5,3},
    {0x36,0x49,0x49,0x49,0x36}, {6,0x49,0x49,0x29,0x1e},
    {0,0x36,0x36,0,0}, {0,0x56,0x36,0,0}, {8,0x14,0x22,0x41,0},
    {0x14,0x14,0x14,0x14,0x14}, {0,0x41,0x22,0x14,8},
    {2,1,0x51,9,6}, {0x32,0x49,0x79,0x41,0x3e},
    {0x7e,0x11,0x11,0x11,0x7e}, {0x7f,0x49,0x49,0x49,0x36},
    {0x3e,0x41,0x41,0x41,0x22}, {0x7f,0x41,0x41,0x22,0x1c},
    {0x7f,0x49,0x49,0x49,0x41}, {0x7f,9,9,9,1},
    {0x3e,0x41,0x49,0x49,0x7a}, {0x7f,8,8,8,0x7f},
    {0,0x41,0x7f,0x41,0}, {0x20,0x40,0x41,0x3f,1},
    {0x7f,8,0x14,0x22,0x41}, {0x7f,0x40,0x40,0x40,0x40},
    {0x7f,2,0x0c,2,0x7f}, {0x7f,4,8,0x10,0x7f},
    {0x3e,0x41,0x41,0x41,0x3e}, {0x7f,9,9,9,6},
    {0x3e,0x41,0x51,0x21,0x5e}, {0x7f,9,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31}, {1,1,0x7f,1,1},
    {0x3f,0x40,0x40,0x40,0x3f}, {0x1f,0x20,0x40,0x20,0x1f},
    {0x3f,0x40,0x38,0x40,0x3f}, {0x63,0x14,8,0x14,0x63},
    {7,8,0x70,8,7}, {0x61,0x51,0x49,0x45,0x43}
};

static uint8_t column;
static uint8_t line;

void oled_command(void) {
    PORTB &= ~(1 << DC);
}

void oled_write_command(uint8_t command) {
    oled_command();
    spi_select(SPI_SLAVE_OLED);
    spi_write(command);
    spi_deselect();
}

void oled_goto_column(uint8_t col) {
    if (col >= OLED_WIDTH) return;
    oled_write_command(OLED_CMD_SET_LOWER_COLUMN_MASK | (col & 0x0f));
    oled_write_command(OLED_CMD_SET_HIGHER_COLUMN_MASK | (col >> 4));
    column = col;
}

void oled_goto_line(uint8_t page) {
    if (page >= OLED_TOTAL_PAGES) return;
    oled_write_command(OLED_CMD_SET_PAGE_START_MASK | page);
    line = page;
}

void oled_home(void) {
    oled_goto_line(0);
    oled_goto_column(0);
}

void oled_reset(void) {
    for (uint8_t page = 0; page < OLED_TOTAL_PAGES; page++) {
        oled_goto_line(page);
        oled_goto_column(0);
        PORTB |= (1 << DC);
        spi_select(SPI_SLAVE_OLED);
        for (uint8_t col = 0; col < OLED_WIDTH; col++) spi_write(0);
        spi_deselect();
    }
    oled_home();
}

void oled_init(void) {
    DDRB |= (1 << DC);
    const uint8_t commands[] = {
        OLED_CMD_SET_COMMAND_LOCK, OLED_CMD_LOCK_UNLOCK_VAL,
        OLED_CMD_SET_DISPLAY_OFF,
        OLED_CMD_SET_DISPLAY_CLOCK, 0x80,
        OLED_CMD_SET_MULTIPLEX_RATIO, OLED_HEIGHT - 1,
        OLED_CMD_SET_DISPLAY_OFFSET, 0,
        OLED_CMD_SET_START_LINE_MASK,
        OLED_CMD_SET_MEMORY_ADDR_MODE, OLED_ADDR_MODE_PAGE,
        OLED_CMD_SET_SEGMENT_REMAP_127, OLED_CMD_SET_COM_SCAN_REMAPPED,
        OLED_CMD_SET_COM_PINS_CONFIG, 0x12,
        OLED_CMD_SET_CONTRAST, 0x7f,
        OLED_CMD_SET_PRECHARGE_PERIOD, 0x22,
        OLED_CMD_SET_VCOMH_DESELECT, OLED_VCOMH_0_78_VCC,
        OLED_CMD_ENTIRE_DISPLAY_RAM, OLED_CMD_SET_NORMAL_DISPLAY,
        OLED_CMD_DEACTIVATE_SCROLL
    };
    for (uint8_t i = 0; i < sizeof(commands); i++)
        oled_write_command(commands[i]);
    oled_reset();
    oled_write_command(OLED_CMD_SET_DISPLAY_ON);
}

void oled_print(const char *text) {
    if (!text) return;
    while (*text) {
        uint8_t ch = (uint8_t)*text++;
        if (ch == '\r') {
            oled_goto_column(0);
            continue;
        }
        if (ch == '\n' || column > OLED_WIDTH - 6) {
            oled_goto_line((line + 1) % OLED_TOTAL_PAGES);
            oled_goto_column(0);
            if (ch == '\n') continue;
        }
        if (ch >= 'a' && ch <= 'z') ch -= 'a' - 'A';
        if (ch < ' ' || ch > 'Z') ch = '?';
        PORTB |= (1 << DC);
        spi_select(SPI_SLAVE_OLED);
        for (uint8_t i = 0; i < 5; i++)
            spi_write(pgm_read_byte(&font[ch - ' '][i]));
        spi_write(0);
        spi_deselect();
        column += 6;
    }
}
