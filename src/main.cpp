/*
 * main.cpp
 *
 *  Created on: 01-Aug-2019
 *      Author: sharath
 */


#include "paging.h"
#pragma warning(disable : 4996)
#define RECT_WIDTH 30
#define RECT_HEIGHT 17
#define SPEED 0.5
using namespace std;

struct coord {
	float x, y;
	char id[10] = "";
};
struct frm_pg {
	float x, y;
	int page_no;
	bool empty = true;
};
struct coord page[30], page_copy[30], *frame, *frame_copy;
struct frm_pg page_table[10][30];
int no_frame=3, no_page=10;
struct coord *source, *source_copy;
struct coord *dest, *dest_copy;

//////////////////////////////////////////////////////////////////////////
int example[20] = { 1,2,3,4,2,1,5,6,2,1,2,3,7,6,3,2,1,2,3,6 };
int ref_id[30] = {1,2,3,4,2,1,5,6,2,1,2,3,7,6,3,2,1,2,3,6};
int n=20, max_index=0, frm_counter=0;
int time1[30], page_fault = 0, choice = 1, k=0, curr = -1, hit=-1;
bool moving = false, frm_count_start = false, page_hit[30] = { false };
char hr[5];
int   main_window;
bool start = false;
//////////////////////////////////////////////////////////////////////////
GLUI* glui;
GLUI_RadioGroup* radio;
GLUI_Spinner* spinner;
GLUI_EditText* edittext;
GLUI_StaticText* errortext;
GLUI_Button *play, *restart;
bool error = false;


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(1000, 700);

	main_window = glutCreateWindow("Paging");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyhandle);
	GLUI_Master.set_glutReshapeFunc(reshape);

	glui = GLUI_Master.create_glui_subwindow(main_window,
		GLUI_SUBWINDOW_RIGHT);

	glui->add_statictext("");
	/******** Choose paging algorithm ********/
	GLUI_Panel* paging = glui->add_panel("Paging Algorithm");
	radio = glui->add_radiogroup_to_panel(paging, NULL, 0, control_cb);
	glui->add_radiobutton_to_group(radio, "FIFO");
	glui->add_radiobutton_to_group(radio, "LRU");
	glui->add_radiobutton_to_group(radio, "OPTIMAL");
	glui->add_statictext("");
	glui->add_statictext("");

	/******* Choose Main memory size ********/
	GLUI_Panel* mem_size = glui->add_panel("Memory Size");
	spinner = glui->add_spinner_to_panel(mem_size, "NO. Frames: ", 2, NULL, 1, control_cb);
	spinner->set_int_limits(1, 10);
	spinner->set_int_val(3);
	glui->add_statictext("");
	glui->add_statictext("");

	/**********Enter reference id **********/
	GLUI_Panel* ref_ids = new  GLUI_Panel(glui, "Reference ID (Max len: 30)");
	edittext = glui->add_edittext_to_panel(ref_ids, "Ref_id(0-9)", 1, 0, 2, control_cb);
	edittext->set_w(160);
	edittext->set_h(20);
	edittext->set_alignment(GLUI_ALIGN_LEFT);
	glui->add_statictext_to_panel(ref_ids, "");
	errortext = glui->add_statictext_to_panel(ref_ids, "Invalid input");
	errortext->hidden = true;
	errortext->set_alignment(GLUI_ALIGN_CENTER);
	glui->add_statictext("");
	glui->add_statictext("");

	/*** Disable/Enable buttons ***/
	play = glui->add_button("START", 3, control_cb);
	restart = glui->add_button("RESTART", 4, control_cb);
	restart->disable();

	glui->add_statictext("");
	glui->add_statictext("");
	/****** A 'quit' button *****/
	glui->add_button("Quit", -1, (GLUI_Update_CB)exit);
	glui->add_statictext("");
	glui->add_statictext("Press");
	glui->add_statictext("enter - paging");
	glui->add_statictext("'s' - search replacement frm");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("");
	glui->add_statictext("     BY");
	glui->add_statictext("     SHARATH B P");
	/**** Link windows to GLUI, and register idle callback ******/
	glui->set_main_gfx_window(main_window);

	glutMainLoop();

	return EXIT_SUCCESS;
}

void init() {

	max_index = 0; frm_counter = 0;
	page_fault = 0; k = 0; curr = -1; hit = -1;
	for(int i=0;i<n;i++)
		for (int j = 0;j < no_frame;j++)
		page_table[j][i].empty = true;

	glClearColor(1, 1, 1, 1);

	frame = new coord[10];
	frame_copy = new coord[10];
	
	for (int i = 0; i < n;i++)
		page_hit[i] = false;
	
	//Initialize time1
	for (int i = 0;i < no_frame;i++) {
		time1[i] = no_frame - i;
	}

	// Initilize Secondary storage (pages)
	for(int i=0;i<no_page;i++){
		page_copy[i].x = page[i].x = 370;
		page_copy[i].y = page[i].y = 450 - i * RECT_HEIGHT;
		sprintf(page[i].id, "%s %d", "Page", i);
		sprintf(page_copy[i].id, "%s %d", "Page", i);
	}

	// Initialize Primary Memory (frames)
	for (int i = 0;i < 10;i++) {
		frame_copy[i].x = frame[i].x = 300;
		frame_copy[i].y = frame[i].y = 450 - i * RECT_HEIGHT;
		sprintf(frame[i].id, "%s %d", "Frame", i);
		sprintf(frame_copy[i].id, "%s %d", "Frame", i);
	}

}

void control_cb(int control)
{
	int i = 0, dup;
	std::stringstream stream;
	stream.clear();
	stream.str(edittext->get_text());
	error = false;

	switch (control) {
	case 0: choice = radio->get_int_val();
		choice += 1;break;
	case 1: no_frame = spinner->get_int_val();
			glutPostRedisplay();
			break;
	case 2: 
		while (stream >> dup) {
			if (dup >= 0 && dup < 10) {
				ref_id[i++] = dup;
			}
			else {
				error = true;
				i = 0;
				break;
			}

		}
		if (error) {
			errortext->hidden = false;
			break;
		}
		else {
			errortext->hidden = true;
			n = i;
			if(n > 30)
				n = 30;
		}
		if (n == 0) {
			n = 20;
			for (int ii = 0;ii < n;ii++)
				ref_id[ii] = example[ii];
		}
		glutPostRedisplay();break;
	case 3:
		while (stream >> dup) {
			if (dup >= 0 && dup < 10) {
				ref_id[i++] = dup;
			}
			else {
				error = true;
				i = 0;
				break;
			}
		}
		if (error) {
			errortext->hidden = false;
			break;
		}
		else {
			errortext->hidden = true;
			n = i;
			if(n > 30)
				n = 30;
		}
		if (n == 0) {
			n = 20;
			for (int ii = 0;ii < n;ii++)
				ref_id[ii] = example[ii];
		}
		start = true;
		radio->disable();
		spinner->disable();
		edittext->disable();
		play->disable();
		restart->enable();
		SetCursorPos(400, 500);
		glutPostRedisplay();
		break;
	case 4: if (error)
			errortext->enable();
		  else
			errortext->disable();
		start = false;
		radio->enable();
		spinner->enable();
		edittext->enable();
		play->enable();
		restart->disable();
		init();
		glutPostRedisplay();
		break;
	}
}


void reshape(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/*if (w >= h)
		gluOrtho2D(0, 500.0 * (float)w / (float)h, 0, 500.0 * (float)w / (float)h);
	else
		gluOrtho2D(0, 500.0 * (float)h / (float)w, 0, 500.0 * (float)h / (float)w);*/
	gluOrtho2D(0, 500.0 , 0, 500.0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw CPU
	glColor3f(1, 0, 0);
	draw_rectangle(50, 430, 50, 50);
	glColor3f(1, 1, 1);
	draw_text2(37, 425, "CPU");


	// Arrow from CPU to MMU
	glColor3f(0, 0, 0);
	draw_rectangle(100, 430, 15, 1);
	draw_triangle(105, 434);

	// Arrow from MMU to Main memory
	glColor3f(0, 0, 0);
	draw_rectangle(260, 430, 15, 1);
	draw_triangle(265, 434);

	// PD buffer
	glColor3f(0, 0, 0);
	draw_rectangle(130, 430, 30, 20);
	glColor3f(1, 1, 0);
	draw_rectangle(123, 430, 13, 16);
	draw_rectangle(137, 430, 13, 16);
	glColor3f(0, 0, 0);
	char mmu[5];
	sprintf(mmu, "%d", ref_id[k-1]);
	draw_text(122, 427, mmu);
	draw_text(136, 427, "D");

	// FD buffer
	glColor3f(0, 0, 0);
	draw_rectangle(230, 430, 30, 20);
	glColor3f(1, 1, 0);
	draw_rectangle(223, 430, 13, 16);
	draw_rectangle(237, 430, 13, 16);
	glColor3f(0, 0, 0);
	if (hit >= 0)
		sprintf(mmu, "%d", hit);
	else
		sprintf(mmu, "%s", "INV");
	draw_text(219, 427, mmu);
	draw_text(236, 427, "D");

	// Arrow from D to D
	glColor3f(0, 0, 0);
	draw_rectangle(140, 447, 1, 15);
	draw_rectangle(190, 455, 100, 1);
	draw_rectangle(240, 447, 1, 15);
	glPushMatrix();
	glTranslatef(244, 447, 0);
	glRotatef(-90, 0, 0,  1);
	draw_triangle(0, 0);
	glPopMatrix();


	// Draw page table
	glColor3f(0, 0, 0);
	draw_rectangle(180, 300, 50, 180);
	glColor3f(1, 1, 1);
	draw_rectangle(168, 380, 23, 15);
	draw_rectangle(192, 380, 23, 15);
	glColor3f(0, 0, 0);
	draw_text(157, 375, "Pg No.");
	draw_text(182, 375, "Frm No.");

	glPushMatrix();
	for (int temp = 0; temp < no_page; temp++) {
		glTranslatef(0, -16, 0);
		glColor3f(1, 1, 0);
		if (k>0 && k<=n && temp == ref_id[k-1] && moving)
			glColor3f(1, 0, 0);
		if (hit >= 0 && temp == page_table[hit][k-1].page_no)
			glColor3f(0, 1, 0);
		draw_rectangle(168, 380, 23, 15);
		draw_rectangle(192, 380, 23, 15);
		glColor3f(0, 0, 0);
		char tempchr[10];
		sprintf(tempchr, "%s %d", "Pg ", temp);
		draw_text(157, 380, tempchr);
		int temp2;
		for (temp2 = 0; temp2 < no_frame;temp2++) {
			if (k>0 && !page_table[temp2][k-1].empty && page_table[temp2][k-1].page_no == temp) {
				break;
			}
		}
		if (temp2 != no_frame) {
			sprintf(tempchr, "%d", temp2);
			if (page_table[temp2][k - 1].page_no == ref_id[k - 1] && moving)
				sprintf(tempchr, "%s", "INV");
		}
		else
			sprintf(tempchr, "%s", "INV");
		draw_text(182, 380, tempchr);
	}
	glPopMatrix();

	// PD to PT, PT to FD
	if (k == 0) {
		glColor3f(0, 0, 0);
		draw_line(124, 420, -57);
		draw_line(124, 363, -16 * ref_id[k]);
		draw_rectangle(140, 363 - 16 * ref_id[k], 32, 1);
		draw_triangle(150, 367 - 16 * ref_id[k]);

		draw_line(224, 420, -57);
		draw_line(224, 363, -16 * ref_id[k]);
		draw_rectangle(214, 363 - 16 * ref_id[k], 20, 1);
		glPushMatrix();
		glTranslatef(220, 415, 0);
		glRotatef(90, 0, 0, 1);
		draw_triangle(0, 0);
		glPopMatrix();
	}
	else {
		glColor3f(0, 0, 0);
		draw_line(124, 420, -57);
		draw_line(124, 363, -16 * ref_id[k - 1]);
		draw_rectangle(140, 363 - 16 * ref_id[k - 1], 32, 1);
		draw_triangle(150, 367 - 16 * ref_id[k - 1]);

		draw_line((float)224, (float)420, (float)-57);
		draw_line((float)224, (float)363, (float)-16 * ref_id[k - 1]);
		draw_rectangle((float)214, (float)363 - 16 * ref_id[k - 1], (float)20, (float)1);
		glPushMatrix();
		glTranslatef(220, 415, 0);
		glRotatef(90, 0, 0, 1);
		draw_triangle(0, 0);
		glPopMatrix();
	}
	// Draw Pages
	for (int i = 0;i < no_page; i++) {
		glColor3d(1, 0, 0.5);
		draw_rectangle(page_copy[i].x, page_copy[i].y, RECT_WIDTH, RECT_HEIGHT - 2);
		glColor3d(1, 1, 1);
		draw_text(page_copy[i].x - RECT_WIDTH / 2 + 1, page_copy[i].y, page_copy[i].id);
	}

	// Draw Frames
	for (int i=0; i<no_frame; i++) {
		if (frm_counter == i && frm_count_start)
			glColor3d(0, 0, 0.8);
		else if (max_index == i && frm_count_start)
			glColor3d(0.8, 0, 0);
		else
			glColor3d(1, 0.6, 0);
		draw_rectangle(frame_copy[i].x, frame_copy[i].y, RECT_WIDTH, RECT_HEIGHT - 2);
		glColor3f(1, 1, 1);
		draw_text(frame_copy[i].x - RECT_WIDTH / 2 + 1, frame_copy[i].y, frame_copy[i].id);
	}

	for (int i = 0;i < no_page; i++) {
		if (page_table[frm_counter][k-1].page_no == i && !page_table[frm_counter][k-1].empty && frm_count_start)
			glColor3d(0.0, 0.0, 0.8);
		else if (page_table[max_index][k-1].page_no == i && !page_table[max_index][k - 1].empty && frm_count_start)
			glColor3d(0.8, 0.0, 0.0);
		else
			glColor3f(0.5, 0.0, 0.5);
		draw_rectangle(page[i].x, page[i].y, RECT_WIDTH, RECT_HEIGHT - 2);
		glColor3f(1, 1, 1);
		draw_text(page[i].x - RECT_WIDTH / 2 + 1, page[i].y, page[i].id);
	}
	
	//REF ID
	for (int i = 0;i < n;i++) {
		glColor3d(1, 0.3, 0.3);
		if (page_hit[i])
			glColor3f(0, 1, 0);
		draw_rectangle((float)50 + i * 12, (float)185, (float)10, (float)15);
		glColor3f(0, 0, 0);
		draw_num((float)50 + i * 12, (float)185, ref_id[i]);
	}

	//FRAME NO
	glPushMatrix();
	for (int i = 0;i < no_frame;i++) {
		glColor3d(1, 0.6, 0);
		draw_rectangle(25, 167, RECT_WIDTH, RECT_HEIGHT - 4);
		glColor3f(1, 1, 1);
		draw_text(15, 165, frame[i].id);
		glTranslatef(0, -15, 0);
	}
	glPopMatrix();

	//PAGE TABLE
	glPushMatrix();
	glTranslatef(0, 30, 0);
	for (int i = 0;i <= k;i++) {
		for (int j = 0;j < no_frame;j++) {
			if (!page_table[j][i].empty) {
				glColor3f(0.5, 0, 0.5);
				draw_rectangle(page_table[j][i].x, page_table[j][i].y+30, 11, 11);
				glColor3f(1, 1, 1);
				draw_num(page_table[j][i].x-2, page_table[j][i].y+29, page_table[j][i].page_no);
			}
		}
	}
	glPopMatrix();

	if (k >= n) {
		float count = 0;
		for (int i = 0;i < n;i++)
			if (page_hit[i])
				count += 1;
		sprintf(hr, "%.2f", (count / n));
		glColor3f(0, 0, 0);
		draw_text2(300, 50, "HIT RATIO : ");
		draw_text2(370, 50, hr);
	}
	
	glutSwapBuffers();
}

void keyhandle(unsigned char key, int x, int y) {
	switch (key) {
	case 13: if (k < n && !moving && !frm_count_start && start) {
				moving = true;
				if (choice == 1 || choice == 2)
					swap_in();
				else if (choice == 3)
					swap_in2();
				if(!frm_count_start)
					glutTimerFunc(1, move_page, 0);
			}break;
	case 's':if (!moving && start) {
				frm_count_start = true;
				frm_counter = 0;
				max_index = 0;
				glutTimerFunc(1000, frame_selection, 0);
			}break;
	}
}

/////////////////////////////////// TIMER FUNCTIONS ///////////////////////////////

void frame_selection(int value) {
	if (frm_counter < no_frame) {
		glutPostRedisplay();
		
		if (time1[frm_counter] > time1[max_index])
			max_index = frm_counter;
		frm_counter++;
		//cout << "frame counter max " << max_index << endl;
		glutTimerFunc(1000, frame_selection, 0);
	}
	else
		frm_count_start = false;
}

void move_page(int value) {
	if (curr == -1) {
		moving = false;
		glutPostRedisplay();
	}
	else {
		// Page to frame
		if (source->x > dest->x)
			source->x -= SPEED;
		else
			source->x += SPEED;

		if (source->y > dest->y)
			source->y -= SPEED;
		else
			source->y += SPEED;
		if (k > no_frame) {
			if (!page_table[curr][k - 1].empty) {
				// Frame to Page
				if (source_copy->x > dest_copy->x)
					source_copy->x -= SPEED;
				else
					source_copy->x += SPEED;

				if (source_copy->y > dest_copy->y)
					source_copy->y -= SPEED;
				else
					source_copy->y += SPEED;
			}
		}
		glutPostRedisplay();
		if (abs(source->x - dest->x) > SPEED || abs(source->y - dest->y) > SPEED)
			glutTimerFunc(1, move_page, 0);
		else if (k > no_frame) {
			if (abs(source_copy->x - dest_copy->x) > SPEED || abs(source_copy->y - dest_copy->y) > SPEED)
				glutTimerFunc(1, move_page, 0);
			else
				moving = false;
		}
		else
			moving = false;
	}
}

////////////////////////////////// DRAW FUNCTIONS //////////////////////////////////
void draw_rectangle(float x, float y, float w, float h) {
	glBegin(GL_QUADS);
	glVertex2f(x-w/2, y-h/2);
	glVertex2f(x-w/2, y+h/2);
	glVertex2f(x+w/2, y+h/2);
	glVertex2f(x+w/2, y-h/2);
	glEnd();
}

void draw_line(float x, float y, float len)
{
	glBegin(GL_LINES);
	glVertex2f(x, y);
	glVertex2f(x, y + len);
	glEnd();
}

void draw_triangle(float x, float y) {
	glBegin(GL_TRIANGLES);
	glVertex2f(x, y);
	glVertex2f(x + 6, y - 4);
	glVertex2f(x, y - 8);
	glEnd();
}

void draw_num(float x, float y, int num) {
	char temp[5];
	sprintf(temp, "%d", num);
	draw_text(x, y, temp);
}

void draw_text(float x, float y, string s) {
	const char* c = s.c_str();
	int i = 0;
	glRasterPos2f(x, y);
	for (i = 0; c[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c[i]);
}

void draw_text2(float x, float y, string s) {
	const char* c = s.c_str();
	int i = 0;
	glRasterPos2f(x, y);
	for (i = 0; c[i] != '\0'; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c[i]);
}

////////////////////////////////// PAGING ALGORITHM /////////////////////////////////

void swap_in() {
	max_index = 0;
	int i;
	for (i = 0;i < no_frame;i++) {
		time1[i]++;
		if (time1[i] > time1[max_index])
			max_index = i;
	}

	// check hit or miss
	if (k != 0) {
		for (i = 0;i < no_frame;i++) {
			if (page_table[i][k - 1].empty)
				continue;
			if (page_table[i][k - 1].page_no == ref_id[k])
				break;
		}
		if (i == no_frame) {
			// page miss
			hit = -1;
			page_hit[k] = false;
			time1[max_index] = 1;
			curr = max_index;
			for (i = 0;i < no_frame;i++) {
				page_table[i][k].x = page_table[i][k - 1].x + 12;
				page_table[i][k].y = page_table[i][k - 1].y;
				page_table[i][k].page_no = page_table[i][k - 1].page_no;
				page_table[i][k].empty = page_table[i][k - 1].empty;
			}
			page_table[max_index][k].page_no = ref_id[k];
			page_table[max_index][k].empty = false;
			page_fault++;
		}
		else {
			// page hit
			hit = i;
			page_hit[k] = true;
			if (choice == 2)
				time1[i] = 1;
			curr = -1;
			for (i = 0;i < no_frame;i++) {
				page_table[i][k].x = page_table[i][k - 1].x + 12;
				page_table[i][k].y = page_table[i][k - 1].y;
				page_table[i][k].page_no = page_table[i][k - 1].page_no;
				page_table[i][k].empty = page_table[i][k - 1].empty;
			}
		}
	}
	else {
		page_hit[k] = false;
		time1[max_index] = 1;
		curr = max_index;
		page_table[0][k].x = 50;
		page_table[0][k].y = 110;
		page_table[0][k].page_no = ref_id[k];
		page_table[0][k].empty = false;
		for (int i = 1;i < no_frame;i++) {
			page_table[i][k].x = 50;
			page_table[i][k].y = page_table[i - 1][k].y - 15;
		}
	}

	source = &page[ref_id[k]];
	dest = &frame_copy[curr];
	if(k>0)
	if (!page_table[curr][k - 1].empty) {
		source_copy = &page[page_table[max_index][k - 1].page_no];
		dest_copy = &page_copy[page_table[max_index][k - 1].page_no];
	}
	k++;
}

void swap_in2(){
	int fartest = k;
	curr = -1;

	if (k != 0) {
		// hit
		for (int i = 0;i < no_frame;i++)
			if (!page_table[i][k - 1].empty && ref_id[k] == page_table[i][k - 1].page_no) {
				hit = i;
				page_hit[k] = true;
				curr = -1;
				for (i = 0;i < no_frame;i++) {
					page_table[i][k].x = page_table[i][k - 1].x + 12;
					page_table[i][k].y = page_table[i][k - 1].y;
					page_table[i][k].page_no = page_table[i][k - 1].page_no;
					page_table[i][k].empty = page_table[i][k - 1].empty;
				}
				k++;
				return;
			}

		// miss
		for (int i = 0;i < no_frame;i++) {
			if (page_table[i][k - 1].empty) {
				curr = i;
				break;
			}
		}
		if(curr==-1)
			for (int i = 0;i < no_frame;i++) {
				int j;
				for (j = k+1;j < n;j++) {
					if (page_table[i][k - 1].page_no == ref_id[j]) {
						if (j > fartest) {
							fartest = j;
							curr = i;
						}
						break;
					}
				}
				if (j == n) {
					curr = i;
					break;
				}
			}
	
		hit = -1;
		page_hit[k] = false;
		for (int i = 0;i < no_frame;i++) {
			page_table[i][k].x = page_table[i][k - 1].x + 12;
			page_table[i][k].y = page_table[i][k - 1].y;
			page_table[i][k].page_no = page_table[i][k - 1].page_no;
			page_table[i][k].empty = page_table[i][k - 1].empty;
		}
		page_table[curr][k].page_no = ref_id[k];
		page_table[curr][k].empty = false;
		page_fault++;
	}
	else {
		page_hit[k] = false;
		time1[max_index] = 1;
		curr = 0;
		page_table[0][k].x = 50;
		page_table[0][k].y = 110;
		page_table[0][k].page_no = ref_id[k];
		page_table[0][k].empty = false;
		for (int i = 1;i < no_frame;i++) {
			page_table[i][k].x = 50;
			page_table[i][k].y = page_table[i - 1][k].y - 15;
		}
	}


	source = &page[ref_id[k]];
	dest = &frame_copy[curr];
	if (k > 0)
		if (!page_table[curr][k - 1].empty) {
			source_copy = &page[page_table[curr][k - 1].page_no];
			dest_copy = &page_copy[page_table[curr][k - 1].page_no];
		}
	k++;

}

     
