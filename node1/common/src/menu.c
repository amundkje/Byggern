// menu.c

#include "menu.h"
#include "oled.h"
#include "node1_structs.h"
#include "../manager.h"


static enum menuItems current_selected = MENU_START_GAME;
static enum joystick_direction prev_dir = NEUTRAL;


/**
 * This function draw the content of the menu based on selected item.
 */
void menu_draw(void)
{
    oled_home();
    oled_print(current_selected == MENU_START_GAME ? "> Start game" : "  Start game");

    oled_goto_line(1);
    oled_goto_column(0);
    oled_print(current_selected == MENU_CALIBRATE_JOYSTICK ? "> Calibrate joystick" : "  Calibrate joystick"); 

    oled_goto_line(2);
    oled_goto_column(0);
    oled_print(current_selected == MENU_CALIBRATE_MOTOR ? "> Calibrate motor" : "  Calibrate motor");   

    oled_goto_line(3);
    oled_goto_column(0);
    oled_print(current_selected == MENU_CREDITS ? "> Credits" : "  Credits");
}


// Reset menu
void menu_init(void) {
    prev_dir = NEUTRAL;
    current_selected = MENU_START_GAME;
    oled_reset();
    menu_draw();
}


/**
 * This function handles navigating the menu and is called
 * every loop of the main game locig function as long as node 1
 * is in the menu state.
 */
enum menuItems menu_update(void) {

    // Read current joystick direction
    enum joystick_direction dir = joystick_get_direction();

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
            // No sub menu yet
            break;

        // Right nudge selects current menu item
        case RIGHT: 
            return current_selected;

        default:
            break;
        }
    }

    prev_dir = dir;

    return MENU_NONE;
}
