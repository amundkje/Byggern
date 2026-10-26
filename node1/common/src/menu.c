#include "menu.h"
#include "oled.h"
#include "node1_structs.h"
#include "../manager.h"
#include <util/delay.h>
//#define menu_cnt 3

//static uint8_t selected = 0;
//static int8_t previous_menu_direction = 0;

static enum menuItems current_selected = MENU_START_GAME;
static enum joystick_direction prev_dir = NEUTRAL;



void menu_draw(void)
{
    oled_home();
    oled_print(current_selected == MENU_START_GAME ? "> Start game" : "  Start game");

    oled_goto_line(1);
    oled_goto_column(0);
    oled_print(current_selected == MENU_SETTINGS ? "> Settings" : "  Settings");

    oled_goto_line(2);
    oled_goto_column(0);
    oled_print(current_selected == MENU_CALIBRATE ? "> Calibrate joystick" : "  Calibrate joystick");    

    oled_goto_line(3);
    oled_goto_column(0);
    oled_print(current_selected == MENU_CREDITS ? "> Credits" : "  Credits");
}

void menu_init(void) {
    oled_reset();
    menu_draw();
}

enum gameState menu_update(void) {

    // Read current joystick direction
    enum joystick_direction dir = joystick_get_direction();
    enum gameState next_state = MENU;

    // If joystick has not been moved to a new direction, return
    if (dir != prev_dir && dir != NEUTRAL) {

        switch (dir) {
            // Move up
            case UP:
                if (current_selected > 0) {
                    current_selected--;
                    menu_draw();
                }
                break;

            // Move down
            case DOWN:
                if (current_selected < MENU_NUM_ITEMS-1) {
                    current_selected++;
                    menu_draw();
                }
                break;

            // Go back from sub-menu
            case LEFT:
                // To be implemented
                break;

            // Right nudge selects current menu item
            case RIGHT: 
                if (current_selected == MENU_START_GAME) {
                    oled_reset();
                    oled_home();
                    oled_print(" Playing Game ");
                    next_state = RUNNING;
                }
                else if (current_selected == MENU_SETTINGS) {
                    // To be implemented
                }
                else if (current_selected == MENU_CALIBRATE) {
                    // To be implemented
                }
                else if (current_selected == MENU_CREDITS) {
                    // To be implemented
                    oled_reset();
                    oled_home();
                    oled_print("Made by three monkeys");
                    _delay_ms(5000);
                    oled_reset();
                    oled_home();
                    menu_draw();
                }
                break;

            default:
                break;
        }
    }

    prev_dir = dir;

    return next_state;
}






/* Sorry eivind
void menu_select(directional_pos dir){
    if (dir.up && selected > 0){
        selected--;
    }
    else if (dir.down && selected < menu_cnt - 1){
        selected++;
    }
}

void menu_update(directional_pos dir)
{
    int8_t menu_direction = dir.up ? 1 : (dir.down ? -1 : 0);

    // Move once per deflection; release or reverse to move again.
    if (menu_direction != 0 && menu_direction != previous_menu_direction) {
        uint8_t previous_selected = selected;
        menu_select(dir);
        if (selected != previous_selected) {
            menu_draw();
        }
    }
    previous_menu_direction = menu_direction;
}
*/