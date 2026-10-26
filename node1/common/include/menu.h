#ifndef MENU_H
#define MENU_H

#include "node1_structs.h"

enum menuItems {
    MENU_START_GAME = 0,
    MENU_SETTINGS,
    MENU_CALIBRATE,
    MENU_CREDITS,
    MENU_NUM_ITEMS // By having this last; this gives num of items in enum
};

void menu_init(void);
void menu_draw(void);
enum gameState menu_update(void);

// SOrry eivind
//void menu_select(directional_pos dir);
//void menu_draw(void);
//void menu_update(directional_pos dir);

#endif
