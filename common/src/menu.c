#include "menu.h"
#include "oled.h"
#include "global_structs.h"
#define menu_cnt 3
static uint8_t selected = 0;
static int8_t previous_menu_direction = 0;
void menu_draw(void)
{
    oled_home();
    oled_print(selected == 0 ? "> Start game" : "  Start game");

    oled_goto_line(1);
    oled_goto_column(0);
    oled_print(selected == 1 ? "> Settings" : "  Settings");

    oled_goto_line(2);
    oled_goto_column(0);
    oled_print(selected == 2 ? "> Options" : "  Options");
}

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
