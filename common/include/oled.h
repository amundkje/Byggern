#include <stdint.h>
#include <avr/io.h>

#define DC  PB2
#ifndef OLED_COMMANDS_H
#define OLED_COMMANDS_H

/* ============================================================
 * 1. Display Dimensions and RAM Structure
 * ============================================================ */
#define OLED_WIDTH                  128     // Display width in pixels
#define OLED_HEIGHT                 64      // Display height in pixels
#define OLED_TOTAL_PAGES                8       // GDDRAM pages: PAGE0-PAGE7
#define OLED_MAX_COLUMN                 127     // Column range: 0-127
/* ============================================================
 * 2. Fundamental Command Set
 * ============================================================ */
#define OLED_CMD_SET_CONTRAST           0x81    // Followed by contrast byte 0x00-0xFF
#define OLED_CMD_ENTIRE_DISPLAY_RAM     0xA4    // Resume display from GDDRAM contents
#define OLED_CMD_ENTIRE_DISPLAY_ON      0xA5    // Entire display ON, ignoring RAM
#define OLED_CMD_SET_NORMAL_DISPLAY     0xA6    // Normal display
#define OLED_CMD_SET_INVERSE_DISPLAY    0xA7    // Inverse display
#define OLED_CMD_SET_DISPLAY_OFF        0xAE    // Display OFF / Sleep Mode
#define OLED_CMD_SET_DISPLAY_ON         0xAF    // Display ON / Normal Mode
#define OLED_CMD_NOP                    0xE3    // No Operation

#define OLED_CMD_SET_COMMAND_LOCK       0xFD    // Command Lock setup
#define OLED_CMD_LOCK_UNLOCK_VAL        0x12    // Unlock command interface
#define OLED_CMD_LOCK_LOCK_VAL          0x16    // Lock command interface
/* ============================================================
 * 3. Addressing Setting Commands
 * ============================================================ */
#define OLED_CMD_SET_LOWER_COLUMN_MASK  0x00    // Lower column nibble base
#define OLED_CMD_SET_HIGHER_COLUMN_MASK 0x10    // Higher column nibble base

#define OLED_CMD_SET_MEMORY_ADDR_MODE   0x20    // Set memory addressing mode

#define OLED_ADDR_MODE_HORIZONTAL       0x00    // Horizontal Addressing Mode
#define OLED_ADDR_MODE_VERTICAL         0x01    // Vertical Addressing Mode
#define OLED_ADDR_MODE_PAGE             0x02    // Page Addressing Mode

#define OLED_CMD_SET_COLUMN_ADDR        0x21    // Set column start/end address
#define OLED_CMD_SET_PAGE_ADDR          0x22    // Set page start/end address

#define OLED_CMD_SET_PAGE_START_MASK    0xB0    // Page start address base (0xB0-0xB7)
/* ============================================================
 * 4. Hardware Configuration Commands
 * ============================================================ */
#define OLED_CMD_SET_START_LINE_MASK    0x40    // Display start line base (0-63)

#define OLED_CMD_SET_SEGMENT_REMAP_0    0xA0    // Column 0 mapped to SEG0
#define OLED_CMD_SET_SEGMENT_REMAP_127  0xA1    // Column 127 mapped to SEG0

#define OLED_CMD_SET_MULTIPLEX_RATIO    0xA8    // Set MUX ratio

#define OLED_CMD_SET_COM_SCAN_NORMAL    0xC0    // COM0 -> COM[N-1]
#define OLED_CMD_SET_COM_SCAN_REMAPPED  0xC8    // COM[N-1] -> COM0

#define OLED_CMD_SET_DISPLAY_OFFSET     0xD3    // Set vertical display shift
#define OLED_CMD_SET_COM_PINS_CONFIG    0xDA    // Set COM pins configuration
#define OLED_CMD_SET_GPIO               0xDC    // Set GPIO state
/* ============================================================
 * 5. Timing and Driving Scheme Commands
 * ============================================================ */
#define OLED_CMD_SET_DISPLAY_CLOCK      0xD5    // Clock divide ratio / oscillator frequency
#define OLED_CMD_SET_PRECHARGE_PERIOD   0xD9    // Pre-charge period
#define OLED_CMD_SET_VCOMH_DESELECT     0xDB    // VCOMH deselect level

#define OLED_VCOMH_0_64_VCC             0x00    // ~0.64 * VCC
#define OLED_VCOMH_0_78_VCC             0x34    // ~0.78 * VCC
#define OLED_VCOMH_0_84_VCC             0x3C    // ~0.84 * VCC
/* ============================================================
 * 6. Scrolling Command Set
 * ============================================================ */
#define OLED_CMD_SCROLL_RIGHT           0x26    // Continuous right horizontal scroll
#define OLED_CMD_SCROLL_LEFT            0x27    // Continuous left horizontal scroll

#define OLED_CMD_SCROLL_VERT_RIGHT      0x29    // Vertical + right horizontal scroll
#define OLED_CMD_SCROLL_VERT_LEFT       0x2A    // Vertical + left horizontal scroll

#define OLED_CMD_DEACTIVATE_SCROLL      0x2E    // Stop scrolling
#define OLED_CMD_ACTIVATE_SCROLL        0x2F    // Start scrolling

#define OLED_CMD_SET_VERTICAL_SCROLL    0xA3    // Set vertical scroll area

#define OLED_CMD_CONTENT_SCROLL_RIGHT   0x2C    // Scroll content right by one column
#define OLED_CMD_CONTENT_SCROLL_LEFT    0x2D    // Scroll content left by one column

// Call spi_init() before oled_init(). Lines are pages (0-7).
void oled_command(void);
void oled_write_command(uint8_t command);
void oled_init(void);
void oled_reset(void);
void oled_home(void);
void oled_print(const char *text);
void oled_goto_column(uint8_t col);
void oled_goto_line(uint8_t page);

#endif /* OLED_COMMANDS_H */
