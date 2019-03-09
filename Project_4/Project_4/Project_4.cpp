#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "vec.h"

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define MAX 4

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool active=true;

typedef struct {
    float r, g, b;
} color_t;


typedef struct
{
    int x ,y;
}random_t;

typedef struct {
    vec_t   pos;
    color_t color;
    vec_t   vel;
    bool visible;
} light_t;

typedef struct {
    vec_t pos;
    vec_t N;
} vertex_t;

typedef struct {
    float x, y;
    float gravity, slope, radius;
    bool direction;
    vec_t VEC;
    vertex_t p;
    color_t planetColor;
}planets_t;

light_t light[MAX+1];
planets_t planets[MAX];

random_t randomLight[3];
random_t randomSpeed[3];

color_t mulColor(float k, color_t c) {
    color_t tmp = { k * c.r, k * c.g, k * c.b };
    return tmp;
}

color_t addColor(color_t c1, color_t c2) {
    color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
    return (-1.0 / 350.0) * d/2 + 1.0;
}

color_t calculateColor(light_t source, vertex_t v) {
    vec_t L = subV(source.pos, v.pos);
    vec_t uL = unitV(L);
    float factor = dotP(uL, v.N) * distanceImpact(magV(L));
    return mulColor(factor, source.color);
}
//
// to draw circle, center at (x,y)
// radius r
//
void circle( int x, int y, int r )
{
#define PI 3.1415
    float angle ;
    glBegin( GL_POLYGON ) ;
    for ( int i = 0 ; i < 100 ; i++ )
    {
        angle = 2*PI*i/100;
        glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
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

void print(int x, int y, char *string, void *font )
{
    int len, i ;
    
    glRasterPos2f( x, y );
    len = (int) strlen( string );
    for ( i =0; i<len; i++ )
    {
        glutBitmapCharacter( font, string[i]);
    }
}

// display text with variables.
void vprint(int x, int y, void *font, char *string , ...)
{
    va_list ap;
    va_start ( ap, string );
    char str[1024] ;
    vsprintf( str, string, ap ) ;
    va_end(ap) ;
    
    int len, i ;
    glRasterPos2f( x, y );
    len = (int) strlen( str );
    for ( i =0; i<len; i++ )
    {
        glutBitmapCharacter( font, str[i]);
    }
}

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
void findRandLight(random_t r[])
{
    for(int i=0; i<MAX; i++)
    {
        r[i].x = rand()%780-390;
        r[i].y = rand()%700-350;
    }
}
void findRandSpeed(random_t r[])
{
    for(int i=0; i<3; i++)
    {
        r[i].x = (rand() % (5+1 -(-5)) + -5);
        r[i].y = (rand() % (5+1 -(-5)) + -5);
        
        if (r[i].x == 0)
            r[i].x = (rand() % (5+1 -(-5)) + -5);
        if (r[i].y == 0)
            r[i].y = (rand() % (5+1 -(-5)) + -5);
    }
}
void RESTART()
{
    findRandLight(randomLight);
    findRandSpeed(randomSpeed);
    {
        light[0]  = { { (double)randomLight[0].x, (double)randomLight[0].y }, { 1, 0, 0 }, { (double)randomSpeed[0].x,  (double)randomSpeed[0].y},true },
        light[1] = { {  (double)randomLight[1].x, (double)randomLight[1].y }, { 0, 1, 0 }, {(double)randomSpeed[1].x, (double)randomSpeed[1].y},true },
        light[2] = { { (double)randomLight[2].x, (double)randomLight[2].y }, { 0, 0, 1 }, { (double)randomSpeed[2].x, (double)randomSpeed[2].y},true },
        //SUN
        light[3] = { { 0, 0 }, { 1, 1, 1 }, { 3, -2},true };
    };
    
    for (int i = 0; i < MAX-1; i++) {
        planets[i].gravity = rand() % 3 + 1;
        planets[i].slope = rand() % 361;
        planets[i].direction = rand() % 2;
        planets[i].radius = rand() % 6 + 25;
    }
}
void Init() {
    
    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    srand(time(NULL));
    RESTART();
    
}

//To calculate the variables of the planet
void planetCalc(planets_t pla)
{
    int i;
    
    for (i = 0; i <= 100; i++)
    {
        pla.slope = 2 * PI * i / 100;
        pla.VEC = { pla.x + pla.radius*cos(pla.slope), pla.y + pla.radius*sin(pla.slope) };
        pla.p = { { pla.VEC.x, pla.VEC.y } };
        pla.p.N = unitV(subV({ pla.VEC.x, pla.VEC.y }, { pla.x, pla.y }));
        
        // clear the planetColors
        pla.planetColor = { 0, 0, 0 };
        
        
        //light reflections on the planet
        for (int k = 0; k < MAX; k++)
        {
            if (light[k].visible == true)
                pla.planetColor = addColor(pla.planetColor, calculateColor(light[k], pla.p));
            
        }
        
        //give the calculated color to planets
        glColor3f(pla.planetColor.r, pla.planetColor.g, pla.planetColor.b);
        
        //spawn the objects with their properties
        glVertex2f(pla.VEC.x, pla.VEC.y);
    }
}
//To display Planets
void spawnPlanets(planets_t planets, float radius) {
    
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.3, 0.3, 0.3);
    
    // do the necessary calculation
    planets.x = radius * cos(planets.slope * D2R);
    planets.y = radius * sin(planets.slope * D2R);
    
    // check its slope to move it that direction
    if (planets.direction == 1)
        planets.x *= -1;
    
    // spawn the first white planetColor color of object
    glVertex2f(planets.x, planets.y);
    
    planetCalc(planets);
    
    glEnd();
}

void menu()
{
    //Bottom side
    glColor3f(1,1,1);
    glRectf(-400, -360, 400, -400);
    //On/Off check
    glColor3f(0,0,0);
    if(light[0].visible)
        vprint(-390, -385, GLUT_BITMAP_8_BY_13, "F1(RED):ON");
    else
        vprint(-390, -385, GLUT_BITMAP_8_BY_13, "F1(RED):OFF");
    if(light[1].visible)
        vprint(-260, -385, GLUT_BITMAP_8_BY_13, "F2(GREEN):ON");
    else
        vprint(-260, -385, GLUT_BITMAP_8_BY_13, "F2(GREEN):OFF");
    if(light[2].visible)
        vprint(-130, -385, GLUT_BITMAP_8_BY_13, "F3(BLUE):ON");
    else
        vprint(-130, -385, GLUT_BITMAP_8_BY_13, "F3(BLUE):OFF");
    if(light[3].visible)
        vprint(0, -385, GLUT_BITMAP_8_BY_13, "F4(SUN):ON");
    else
        vprint(0, -385, GLUT_BITMAP_8_BY_13, "F4(SUN):OFF");
    if(active)
        vprint(130, -385, GLUT_BITMAP_8_BY_13, "F5(ANIMATION):ON");
    else
        vprint(130, -385, GLUT_BITMAP_8_BY_13, "F5(ANIMATION):OFF");
    vprint(280, -385, GLUT_BITMAP_8_BY_13, "F6(RESTART)");
}
void drawSunMenu()
{
    
    //Buttons to change the color of the sun
    vprint(230, 360, GLUT_BITMAP_8_BY_13, "BUTTONS TO");
    vprint(230, 340, GLUT_BITMAP_8_BY_13, "CHANGE COLOR");
    vprint(330, 350, GLUT_BITMAP_8_BY_13, "->");
    glColor3f(1,1,1);
    glRectf(360, 360, 380, 380);
    glRectf(360, 340, 380, 320);
    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(370, 375);
    glVertex2f(365, 365);
    glVertex2f(375, 365);
    glVertex2f(370, 325);
    glVertex2f(365, 335);
    glVertex2f(375, 335);
    glEnd();
    
}
void displayLightSources(){
    // light sources
    for (int i = 0; i < MAX; i++)
    {
        if(light[i].visible)
        {
            glColor3f(light[i].color.r, light[i].color.g, light[i].color.b);
            if(i != MAX-1)
                circle(light[i].pos.x, light[i].pos.y, 10);
            else
                circle(light[i].pos.x, light[i].pos.y, 50);
        }
    }
}
void display() {
    
    //
    // clear window to black
    //
    glClearColor( 0, 0 , 0 , 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    
    //draw sun func
    drawSunMenu();
    
    //draw menu func
    menu();
    
    //to calculate distance between planets
    float radius = 135;
    
    //display objects at different distance
    for (int i = 0; i < MAX-1; i++) {
        spawnPlanets(planets[i], radius);
        radius += 75;
    }
    
    displayLightSources();
   
    glutSwapBuffers();
}


//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y )
{
    // exit when ESC is pressed.
    if ( key == 27 )
        exit(0);
    
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

void onKeyUp(unsigned char key, int x, int y )
{
    // exit when ESC is pressed.
    if ( key == 27 )
        exit(0);
    
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown( int key, int x, int y )
{
    // Write your codes here.
    switch (key) {
        case GLUT_KEY_UP: up = true; break;
        case GLUT_KEY_DOWN: down = true; break;
        case GLUT_KEY_LEFT: left = true; break;
        case GLUT_KEY_RIGHT: right = true; break;
    }
    
    if(key==GLUT_KEY_F1)
        light[0].visible= !light[0].visible;
    
    if(key==GLUT_KEY_F2)
        light[1].visible= !light[1].visible;
    
    if(key==GLUT_KEY_F3)
        light[2].visible= !light[2].visible;
    
    if(key==GLUT_KEY_F4)
        light[3].visible= !light[3].visible;
    
    if(key==GLUT_KEY_F5)
        active= !active;
    
    if(key==GLUT_KEY_F6)
        RESTART();
    
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp( int key, int x, int y )
{
    // Write your codes here.
    switch (key) {
        case GLUT_KEY_UP: up = false; break;
        case GLUT_KEY_DOWN: down = false; break;
        case GLUT_KEY_LEFT: left = false; break;
        case GLUT_KEY_RIGHT: right = false; break;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//

//Functions to check the cursor whether its inside the buttons or not
bool insideTop(int x, int y) {
    return (x > 760 && x < 780 && y > 20 && y < 40);
}
bool insideBot(int x, int y) {
    return (x > 760 && x < 780 && y > 60 && y < 80);
}
void onClick( int button, int stat, int x, int y )
{
    // Write your codes here.
    
    if(button==GLUT_LEFT_BUTTON && stat==GLUT_DOWN)
    {
        if(insideTop(x,y))
        {
            if(light[3].color.b>0)
                light[3].color.b-=0.1;
        }
        else if(insideBot(x,y))
            if(light[3].color.b<1)
                light[3].color.b+=0.1;
    }
    
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize( int w, int h )
{
    winWidth = w;
    winHeight = h;
    glViewport( 0, 0, w, h ) ;
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -w/2, w/2, -h/2, h/2, -1, 1);
    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown( int x, int y ) {
    // Write your codes here.
    
    
    
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

void onMove( int x, int y ) {
    // Write your codes here.
    
    
    
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

#if TIMER_ON == 1
void onTimer( int v ) {
    
    glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
    // Write your codes here.
    
    if(active)
    {
        for (int i = 0; i < MAX-1; i++)
        {
            light[i].pos = addV(light[i].pos, light[i].vel);
            
            // Reflection from Walls.
            if (light[i].pos.x > 390 || light[i].pos.x < -390) {
                light[i].vel.x *= -1;
            }
            
            if (light[i].pos.y > 390 || light[i].pos.y < -350) {
                light[i].vel.y *= -1;
            }
            
            // to move the objects
            planets[i].slope += 0.5 * planets[i].gravity;
        }
        
    }
    
    // to refresh the window it calls display() function
    glutPostRedisplay() ; // display()
    
}
#endif



int main( int argc, char *argv[] ) {
    
    glutInit(&argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow( "HW-4: \"Lighting Animation\" by Hakan KOCAK") ;
    
    glutDisplayFunc( display ) ;
    glutReshapeFunc( onResize );
    
    //
    // keyboard registration
    //
    glutKeyboardFunc( onKeyDown ) ;
    glutSpecialFunc( onSpecialKeyDown ) ;
    
    glutKeyboardUpFunc( onKeyUp ) ;
    glutSpecialUpFunc( onSpecialKeyUp ) ;
    
    //
    // mouse registration
    //
    glutMouseFunc( onClick) ;
    glutMotionFunc( onMoveDown ) ;
    glutPassiveMotionFunc( onMove ) ;
    
#if  TIMER_ON == 1
    // timer event
    glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
#endif
    srand(time(NULL));
    Init();
    
    glutMainLoop();
}
