#include <stdio.h>
#include <avr/io.h>
#include "../include/joystick.h"

#define Mid 0
/*
void joystick_calibratex(unsigned int *x) {
    adc_read(&x);
    unsigned int mid_x = *x/x_max;
}
void joystick_calibratey(unsigned int *y) {
    adc_read(&y);
    unsigned int mid_y = *y/y_max;
}
*/
/*
uint8_t joystick_updatex(unsigned int *x) {
}
uint8_t joystick_updatey(unsigned int *y) {
}
enum axis {x, y};
*/
struct Directional_power{
    int Up;
    int Down;
    int Left;
    int Right;
    int Neutral
};
const char *Direction[]={
    "Up",
    "Down",
    "Left",
    "Right",
    "Neutral"
};
enum select {power,direction};


const char *joystick_directional_struct(struct Directional_power *P, enum select, unsigned int x, unsigned int y, int Deadzone) {
    const int mid_min = Mid - Deadzone;
    const int mid_max = Mid + Deadzone;
    int D = 4;
    memset(P, 0, sizeof *P);
        if (x < Mid - Deadzone) {
                P->Left = x;
                D=2;
            } else if (x > mid_max) {
                P->Right = x;
                D=3;
            }
        if (y < Mid - Deadzone) {
                P->Down = y;
                D=1;
            } else if (y > mid_max) {
                P->Up = y;
                D=0;
            }
        P->Neutral = (D==4);
        
    if (select == power){
        return P;
    } else if (select == direction){
        return Direction[D];
    }
}
// Min touchpadvalues? and what type is it?
/*
void joystick_touchpad(unsigned int x, unsigned int y){
}
*/