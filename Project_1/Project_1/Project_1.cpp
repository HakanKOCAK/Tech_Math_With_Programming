#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  700
#define WINDOW_HEIGHT 700
#define SIZE 5

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_ON      1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
bool activeTimer = false;
int  winWidth, winHeight; // current Window width and height
int sign[SIZE] = { 1,1,1,1,1 };
int move[SIZE]; //movement of objects
int winner = 0;
int counter = 0; //time counter
int state = 1; //game state
int alpha; // usage -> final winner
//
// to draw circle, center at (x,y)
// radius r
//

typedef struct
{
    int xC, yC;
}rockets_t;

rockets_t rockets[SIZE];

void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI*i / 100;
        glVertex2f(x + r*cos(angle), y + r*sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI*i / 100;
        glVertex2f(x + r*cos(angle), y + r*sin(angle));
    }
    glEnd();
}

void print(int x, int y, char *string, void *font)
{
    int len, i;
    
    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i<len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
void vprint(int x, int y, void *font, char *string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf(str, string, ap);
    va_end(ap);
    
    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i<len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
void vprint2(int x, int y, float size, char *string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);
    
    int len, i;
    len = (int)strlen(str);
    for (i = 0; i<len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

//
// To display onto window using OpenGL commands
//
void Init()
{
    counter = 0;
    winner = 0;
    int i, a = -295, b = 190;
    for (i = 0; i<SIZE; i++)
        sign[i] = 1;
    
    for (i = 0; i<SIZE; i++)
    {
        rockets[i].xC = a;
        rockets[i].yC = b;
        b -= 120;
    }
}
void display() {
    int i, y;
    //
    // clear window to black
    //
    glClearColor(0, 0.6, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //TOP MENU
    glColor3f(0, 0, 0);
    glRectf(-350, 250, 350, 350);
    glEnd();
    
    //LINES
    glColor3f(0, 1, 0);
    glPointSize(3);
    glBegin(GL_POINTS);
    glEnd();
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex2f(-350, 130);
    glVertex2f(350, 130);
    glVertex2f(-350, 10);
    glVertex2f(350, 10);
    glVertex2f(-350, -110);
    glVertex2f(350, -110);
    glVertex2f(-350, -230);
    glVertex2f(350, -230);
    glEnd();
    
    for (y = 0; y<SIZE; y++)
    {
        //ROCKET MAIN BIDY
        glColor3f(0.3, 0.3, 0.3);
        glRectf(-45 * sign[y] + rockets[y].xC, -12 + rockets[y].yC, 45 * sign[y] + rockets[y].xC, 12 + rockets[y].yC);
        glColor3f(1, 1, 1);
        vprint(-8 * sign[y] + rockets[y].xC, -2 + rockets[y].yC, GLUT_BITMAP_8_BY_13, "%d", y + 1);
        glEnd();
        
        //FIRE
        if (activeTimer)
            if (counter % 2 == 0)
            {
                glColor3f(1, 0.3, 0);
                glRectf(-80 * sign[y] + rockets[y].xC, -7 + rockets[y].yC, -55 * sign[y] + rockets[y].xC, 7 + rockets[y].yC);
                glEnd();
                glColor3f(1, 0.7, 0);
                glRectf(-74 * sign[y] + rockets[y].xC, -3 + rockets[y].yC, -55 * sign[y] + rockets[y].xC, 3 + rockets[y].yC);
                glEnd();
                glColor3f(1, 0.3, 0);
                glBegin(GL_TRIANGLES);
                glVertex2f(-80 * sign[y] + rockets[y].xC, 7 + rockets[y].yC);
                glVertex2f(-80 * sign[y] + rockets[y].xC, -7 + rockets[y].yC);
                glVertex2f(-87 * sign[y] + rockets[y].xC, 0 + rockets[y].yC);
                glColor3f(1, 0.7, 0);
                glVertex2f(-74 * sign[y] + rockets[y].xC, 3 + rockets[y].yC);
                glVertex2f(-74 * sign[y] + rockets[y].xC, -3 + rockets[y].yC);
                glVertex2f(-79 * sign[y] + rockets[y].xC, 0 + rockets[y].yC);
            }
        
        //ROCKET HEAD
        glBegin(GL_TRIANGLES);
        glColor3f(0.9, 0.1, 0);
        glVertex2f(45 * sign[y] + rockets[y].xC, 12 + rockets[y].yC);
        glVertex2f(45 * sign[y] + rockets[y].xC, -12 + rockets[y].yC);
        glVertex2f(70 * sign[y] + rockets[y].xC, 0 + rockets[y].yC);
        
        //ROCKET WINGS
        glVertex2f(-40 * sign[y] + rockets[y].xC, -12 + rockets[y].yC);
        glVertex2f(-10 * sign[y] + rockets[y].xC, -12 + rockets[y].yC);
        glVertex2f(-55 * sign[y] + rockets[y].xC, -20 + rockets[y].yC);
        
        glVertex2f(-40 * sign[y] + rockets[y].xC, 12 + rockets[y].yC);
        glVertex2f(-10 * sign[y] + rockets[y].xC, 12 + rockets[y].yC);
        glVertex2f(-55 * sign[y] + rockets[y].xC, 20 + rockets[y].yC);
        
        //ROCKET BACK
        glColor3f(0.3, 0.3, 0.3);
        glVertex2f(-30 * sign[y] + rockets[y].xC, 0 + rockets[y].yC);
        glVertex2f(-55 * sign[y] + rockets[y].xC, -12 + rockets[y].yC);
        glVertex2f(-55 * sign[y] + rockets[y].xC, 12 + rockets[y].yC);
        
        //TOP WING
        glColor3f(0.9, 0.1, 0);
        glVertex2f(-48 * sign[y] + rockets[y].xC, 3 + rockets[y].yC);
        glVertex2f(-48 * sign[y] + rockets[y].xC, -3 + rockets[y].yC);
        glVertex2f(-10 * sign[y] + rockets[y].xC, 0 + rockets[y].yC);
        glEnd();
        
        glColor3f(0, 0, 0);
        vprint(-55 + rockets[y].xC, 30 + rockets[y].yC, GLUT_BITMAP_8_BY_13, "Speed: %d km/ms", move[y]);
        
    }
    
    
    glColor3f(1, 0, 0);
    vprint(-60, 300, GLUT_BITMAP_TIMES_ROMAN_24, "Rocket Ships");
    glColor3f(1, 1, 1);
    vprint(-300, 300, GLUT_BITMAP_9_BY_15, "TIME");
    vprint(-300, 280, GLUT_BITMAP_8_BY_13, "00:%d:%d", counter / 60, counter % 60);
    if (state == 1)
    {
        vprint(240, 300, GLUT_BITMAP_9_BY_15, "WINNER");
        vprint(260, 280, GLUT_BITMAP_9_BY_15, "%d", winner + 1);
    }
    else
    {
        glColor3f(1, alpha, 0);
        vprint(240, 300, GLUT_BITMAP_9_BY_15, "WINNER");
        vprint(260, 280, GLUT_BITMAP_9_BY_15, "%d", winner + 1);
    }
    glColor3f(1, 1, 0);
    if (state == 1)
        vprint(-170, 270, GLUT_BITMAP_8_BY_13, "Press <SpaceBar> to stop/restart animation");
    else
        vprint(-70, 270, GLUT_BITMAP_8_BY_13, "Press F1 to Restart");
    glutSwapBuffers();
    
}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 32 && state == 1)
        activeTimer = !activeTimer;
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    if (state == 0 && key == GLUT_KEY_F1)
    {
        state = 1;
        Init();
    }
    
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
        case GLUT_KEY_UP: up = false; break;
        case GLUT_KEY_DOWN: down = false; break;
        case GLUT_KEY_LEFT: left = false; break;
        case GLUT_KEY_RIGHT: right = false; break;
    }
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.
    
    
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.
    
    
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
    // Write your codes here.
    
    
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}
int findWinner()
{
    int i;
    
    for (i = 0; i<SIZE; i++)
    {
        if (sign[i] == 1)
        {
            if (sign[winner] == 1)
                if (rockets[i].xC > rockets[winner].xC && rockets[i].xC)
                    winner = i;
        }
        else
        {
            if (sign[winner] == 1)
                winner = i;
            else if (rockets[i].xC < rockets[winner].xC && rockets[i].xC)
                winner = i;
        }
    }
    return(i);
}
#if TIMER_ON == 1
void onTimer(int v)
{
    int i;
    
    glutTimerFunc(TIMER_PERIOD / 20, onTimer, 0);
    // Write your codes here.
    srand(time(0));
    if (activeTimer && state == 1)
    {
        counter += 3;
        for (i = 0; i<SIZE; i++)
        {
            if (sign[i] == 1)
            {
                if (rockets[i].xC + 70 <350)
                {
                    move[i] = rand() % 8 + 1;
                    rockets[i].xC += move[i];
                    findWinner();
                }
                else
                    sign[i] = -1;
            }
            if (sign[i] == -1)
            {
                if (rockets[i].xC - 70 > -350)
                {
                    move[i] = rand() % 8 + 1;
                    rockets[i].xC -= move[i];
                    findWinner();
                }
                else
                {
                    activeTimer = false;
                    state = 0;
                }
            }
        }
    }
    
    if (state == 0)
        if (alpha == 0)
            alpha = 1;
        else
            alpha = 0;
    
    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()
    
}
#endif

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Racing Game");
    
    glutDisplayFunc(display);
    glutReshapeFunc(onResize);
    
    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);
    
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);
    
    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);
    
#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif
    
    Init();
    
    glutMainLoop();
    
}
