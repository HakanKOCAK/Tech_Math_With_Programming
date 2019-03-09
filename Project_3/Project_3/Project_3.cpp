#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  20 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define SIZE 3

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool fire=false; // checks whether the bullet is fired or not
int counter;//time
int minus;//time
int timer;//time

bool activeTimer=false;//timer check

int totalPoints;
int ExtraPoints;
int collisionPoints;


int state=0; // Game state

typedef struct
{
    float x,y, angle;
}bullets_t;

typedef struct
{
    float angle, x, y;
}mouse_t;
mouse_t mouse;
bullets_t bullet;

typedef struct
{
    float x,  y, angle;
    
}weapon_t;
weapon_t weapon;
typedef struct
{
    float y, x;
}center_t;
typedef struct
{
    int r, g, b;
}colors_t;
typedef struct
{
    center_t center;
    colors_t colors;
    float angle;
    int speed;
    int r;
    int direction;
    int visible;
}objects_t;

objects_t objects[SIZE];
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
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
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
void vertex(weapon_t P, weapon_t Tr, double angle) {
    double xp = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
    double yp = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
    glVertex2d(xp, yp);
}
void Init() {
    
    // Smoothing shapes
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    int i, a=250;
    
    
    activeTimer= false;
    fire=false;
    
    timer=15;
    counter =0;
    minus=0;
    
    totalPoints = 0;
    ExtraPoints = 0;
    collisionPoints = 0;
    weapon.x=0;
    weapon.y=0;
    
    for(i=0; i<SIZE; i++)
    {
        objects[i].r=rand()%20+10; // Random radius
        objects[i].speed=rand()%1+1; //Random speed
        objects[i].angle=rand()%360; //Random start points
        objects[i].colors.r=rand()%256; // Random colors
        objects[i].colors.g=rand()%256; // Random colors
        objects[i].colors.b=rand()%256; // Random colors
        objects[i].center.x=a;
        objects[i].center.y=a;
        objects[i].direction=rand()%2; // Random direction
        objects[i].visible=1;
        a+=50;
    }
    
}
//
// To display onto window using OpenGL commands
//
/*float f(float x) {
 return Amp * sin((fq * x + C) * D2R) + B;
 }*/

void drawBullet()
{
    glColor3f(1,0,0);
    if(fire)
    {
        circle(bullet.x, bullet.y, 6);
        if(bullet.x >400 || bullet.x <-400 || bullet.y >400 || bullet.y < -400)
            fire = !fire;
    }
    
    
}
void drawBackGround()
{
    glColor3f(0.8,0.5,0.2);
    glBegin(GL_LINES);
    glVertex2f(0, 400);
    glVertex2f(0, -400);
    glVertex2f(400, 0);
    glVertex2f(-400, 0);
    glEnd();
    
    glColor3f(0.3, 0.9, 0.5);
    circle_wire(0, 0, 350);
    circle_wire(0, 0, 300);
    circle_wire(0, 0, 250);
    
    
    
}
void drawObjects()
{
    int i;
    if(activeTimer)
    {
        
        for(i=0; i<SIZE; i++)
        {
            if(objects[i].visible==1)
            {
                glColor3ub(objects[i].colors.r,objects[i].colors.g,objects[i].colors.b);
                circle(objects[i].center.x*cos(objects[i].angle* D2R) , objects[i].center.y*sin(objects[i].angle*D2R) , objects[i].r);
                
                glColor3f(0, 0, 0);
                vprint(objects[i].center.x*cos(objects[i].angle* D2R)-objects[i].r/2, objects[i].center.y*sin(objects[i].angle*D2R)-objects[i].r/4,GLUT_BITMAP_8_BY_13, "%0.0f",objects[i].angle);
            }
        }
        
    }
}
void drawWeapon()
{
    glColor3f(1, 1, 1);
    glBegin(GL_TRIANGLES);
    vertex({40,0}, {0,0}, weapon.angle*D2R);
    vertex({0,25}, {0,0}, weapon.angle*D2R);
    vertex({0,-25}, {0,0}, weapon.angle*D2R);
    vertex({70,0}, {0,0}, weapon.angle*D2R);
    vertex({30,15}, {0,0}, weapon.angle*D2R);
    vertex({30,-15}, {0,0}, weapon.angle*D2R);
    glEnd();
    glBegin(GL_LINES);
    vertex({65,3},{0,0},weapon.angle*D2R);
    vertex({75,3},{0,0},weapon.angle*D2R);
    vertex({65,-3},{0,0},weapon.angle*D2R);
    vertex({75,-3},{0,0},weapon.angle*D2R);
    
    glEnd();
    glColor3f(1,0,0);
    
    float ang = weapon.angle * D2R;
    if (weapon.angle < 0)
        vprint( 365*cos(ang)-6, 370*sin(ang)-4, GLUT_BITMAP_8_BY_13, "%0.f", weapon.angle + 360);
    else
        vprint( 365*cos(ang)-6, 370*sin(ang)-4, GLUT_BITMAP_8_BY_13, "%0.f", weapon.angle);
}

void displayGameInfo()
{
    glColor3f(1, 1, 1);
    vprint(317, 370, GLUT_BITMAP_HELVETICA_12, "REMAINING");
    vprint(338, 358, GLUT_BITMAP_HELVETICA_12, "TIME");
    glColor3f(0 ,1, 0);
    if(minus==100)
        vprint(334, 341, GLUT_BITMAP_8_BY_13, "%02d:%02d",timer, 99);
    else
        vprint(334, 341, GLUT_BITMAP_8_BY_13, "%02d:%02d",timer, minus);
    
    if(state==0)
    {
        glColor3ub(192, 192, 192);
        glRectf(-200, 150, 200, -150);
        glColor3f(1, 0, 0);
        vprint(-170, 130, GLUT_BITMAP_9_BY_15, "Collision Points: Depends on the radius");
        vprint(-150, 110, GLUT_BITMAP_9_BY_15, "of the targets.");
        
        glColor3ub(0, 0, 0.7);
        vprint(-170, 80, GLUT_BITMAP_9_BY_15, "if r<=16 -- 15 points");
        vprint(-170, 60, GLUT_BITMAP_9_BY_15, "else if r<=23 -- 10 points");
        vprint(-170, 40, GLUT_BITMAP_9_BY_15, "else -- 2 points");
        
        glColor3f(1, 0, 0);
        vprint(-170, -10, GLUT_BITMAP_9_BY_15, "Extra Points: Depends on the remaining");
        vprint(-150, -30, GLUT_BITMAP_9_BY_15, "time.");
        
        glColor3ub(0, 0, 0.7);
        vprint(-170, -60, GLUT_BITMAP_9_BY_15, "if time>=10 -- 10 points");
        vprint(-170, -80, GLUT_BITMAP_9_BY_15, "else if time<=5 -- 5 points");
        
        glColor3f(1, 1, 0);
        vprint(-50, -200, GLUT_BITMAP_HELVETICA_18, "Click To Start");
    }
    
    else if(state==2)
    {
        glColor3ub(192, 192, 192);
        glRectf(-200, 150, 200, -150);
        
        glColor3f(0.9, 0.2, 0);
        vprint(-75, 120, GLUT_BITMAP_HELVETICA_18, "Points From Collisions");
        glColor3ub(0, 102, 102);
        vprint(0, 100, GLUT_BITMAP_9_BY_15, "%02d",collisionPoints);
        
        glColor3f(0.9, 0.2, 0);
        vprint(-40, 10, GLUT_BITMAP_HELVETICA_18, "Extra Points");
        glColor3ub(0, 102, 102);
        vprint(0, -10, GLUT_BITMAP_9_BY_15, "%02d",ExtraPoints);
        
        glColor3f(0.9, 0.2, 0);
        vprint(-40, -80, GLUT_BITMAP_HELVETICA_18, "Total Points");
        totalPoints= ExtraPoints + collisionPoints;
        glColor3ub(0, 102, 102);
        vprint(0, -100, GLUT_BITMAP_9_BY_15, "%02d",totalPoints);
        
        glColor3f(1, 1, 0);
        vprint(-50, -200, GLUT_BITMAP_HELVETICA_18, "Click To Start");
    }
    
}
void display() {
    //
    // clear window to black
    //
    glClearColor( 0, 0 , 0 , 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    drawBackGround();
    drawObjects();
    drawBullet();
    drawWeapon();
    displayGameInfo();
    
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
void onClick( int button, int stat, int x, int y )
{
    // If game is not running
    if(state==0 || state == 2)
    {
        if(button==GLUT_LEFT_BUTTON && stat==GLUT_DOWN)
        {
            state=1;
            Init();
            activeTimer= !activeTimer;
        }
    }
    //If game is running
    else if(state==1 && activeTimer)
        if(button==GLUT_LEFT_BUTTON && stat==GLUT_DOWN)
        {
            if(fire == false)
            {
                fire = !fire;
                bullet.angle=weapon.angle; // equalize angles
                bullet.x=75*cos(weapon.angle*D2R); // bullet coordinates
                bullet.y=75*sin(weapon.angle*D2R); //bullet coordinates
                
            }
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

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove( int x, int y ) {
    // Write your codes here.
    
    weapon.angle=atan2(winHeight/2-y, x-winWidth/2) * 180/PI;
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

#if TIMER_ON == 1
void onTimer( int v ) {
    int i;
    glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
    // Write your codes here.
    if(state==1 && activeTimer) // If the game state is and mouse clicked
    {
        
        if(timer>0 || minus !=0) // If timer > 0
        {
            
            //Timer infos
            if(counter%50==0)
            {
                timer--;
                minus=100;
            }
            counter++;
            minus-=2;
            
            for(i=0; i<SIZE; i++)
                //Rotation of objects
                if(objects[i].direction==1)
                {
                    objects[i].angle+=objects[i].speed;
                    if(objects[i].angle>360)
                        objects[i].angle-=360;
                }
                else
                {
                    objects[i].angle-=objects[i].speed;
                    if(objects[i].angle<0)
                        objects[i].angle+=360;
                }
            
            
            if(fire)
            {
                //Movement of bullet
                bullet.x += 8*cos(bullet.angle*D2R);
                bullet.y += 8*sin(bullet.angle*D2R);
                
                for(i=0; i<SIZE; i++)
                {
                    if(objects[i].visible==1)
                    {
                        //Check the collision
                        float collision = sqrt((bullet.x - objects[i].center.x*cos(objects[i].angle* D2R)) * (bullet.x - objects[i].center.x*cos(objects[i].angle* D2R)) + (bullet.y - objects[i].center.y*sin(objects[i].angle*D2R)) * (bullet.y - objects[i].center.y*sin(objects[i].angle*D2R)));
                        
                        if (collision <= objects[i].r+6)
                        {
                            //Point calculations
                            if(objects[i].r <= 16)
                                collisionPoints+=15;
                            else if (objects[i].r <= 23)
                                collisionPoints+= 10;
                            else
                                collisionPoints += 2;
                            
                            objects[i].visible = 0;
                            
                        }
                    }
                }
            }
            
            if(objects[0].visible==0 && objects[1].visible==0 && objects[2].visible==0)
            {
                state=2;
                fire = !fire;
                if(timer>= 10)
                    ExtraPoints+=10;
                else if(timer >= 5)
                    ExtraPoints+=5;
            }
        }
        else
        {
            state=2;
        }
    }
    // to refresh the window it calls display() function
    glutPostRedisplay() ; // display()
    
}
#endif

int main( int argc, char *argv[] )
{
    srand(time(NULL));
    glutInit(&argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("HW-3: \"Rotating Targets\" by Hakan KOCAK");
    
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
    glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
#endif
    
    Init();
    
    glutMainLoop();
}
