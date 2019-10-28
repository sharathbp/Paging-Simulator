/*
 * paging.h
 *
 *  Created on: 01-Aug-2019
 *      Author: sharath
 */

#include<iostream>
#include<sstream>
#include<list>
#include<string>
#include "GL/glui.h"
#include "GL/glut.h"

using namespace std;

void init();
void control_cb(int);
void reshape(int, int);
void display();
void keyhandle(unsigned char, int, int);

void frame_selection(int);
void move_page(int);

void control_cb(int);
void draw_rectangle(float, float, float, float);
void draw_line(float, float, float);
void draw_triangle(float, float);
void draw_num(float, float, int);
void draw_text(float, float, string );
void draw_text2(float , float , string);

void swap_in();
void swap_in2();

