
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// width and height for paddle
#define PAD_HEIGHT 10
#define PAD_WIDTH 50

// velocity of ball
#define VELOCITY 2.0



// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));

    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // set velocity
    double x_velocity = drand48() + VELOCITY;
    double y_velocity = drand48() + VELOCITY;

    // keep playing until game over
    waitForClick();
       
    
    while(lives > 0 && bricks > 0)
    {
    // paddle follows mouse
        GEvent follow_x = getNextEvent(MOUSE_EVENT);
        if (follow_x != NULL)
        {
            if (getEventType(follow_x) == MOUSE_MOVED)
            {
                double x = getX(follow_x) - getWidth(paddle) / 2;
                double y = HEIGHT - 150;
                setLocation(paddle, x, y);
            }   
        }
        //move ball
        move(ball, x_velocity, y_velocity);
        //move ball left to right
        if (getX(ball) + getWidth(ball) >= getWidth(window))
        {
           x_velocity = -x_velocity;
        }
         else if (getX(ball) <= 0)
        {
            x_velocity = -x_velocity;
        }
        //move ball up and down
        if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            int x = (WIDTH / 2);
            int y = (HEIGHT / 2);
            // y_velocity = -y_velocity;
            lives += -1;
            setLocation(ball, x, y);
            waitForClick();
            
        }
        else if (getY(ball) <= 0)
        {
            y_velocity = -y_velocity;
        }
        pause(10);
        
       // ball collides with object
        GObject object = detectCollision(window, ball);
        if (object != NULL)
        {
            if(strcmp(getType(object), "GLabel") == 0)
            {
                
            }else if (object == paddle)
            {
                y_velocity = -y_velocity;           
            }
            else if (strcmp(getType(object), "GRect") == 0 && object != paddle)
            {
                bricks += -1;
                removeGWindow(window, object); 
                y_velocity = -y_velocity;
                points++;
                updateScoreboard(window, label, points);
                       
            }
        }
       
   }   
              
     

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // make bricks
   for (int i = 0; i < ROWS; i++)
    {
        
        for(int j = 0; j < COLS; j++)
        {
            int br_height = 10;
            int br_width = (WIDTH / COLS)- 5; 
            int br_x = j * (br_width + 5);
            int br_y = i * (br_height + 5);
            
            GRect brick = newGRect(br_x + 5, br_y + 50, br_width, br_height);
            setFilled(brick, true);
            
            //color bricks
            int row_color = i % 5;
            char* col_brick = "ORANGE";
            
            switch(row_color)
            {
                case 0:
                    col_brick = "RED";
                    break;
                case 1:
                    col_brick = "BLUE";
                    break;
                case 2:
                    col_brick = "GREEN";
                    break;
                case 3:
                    col_brick = "YELLOW";
                    break;
                case 4:
                    col_brick = "MAGENTA";
                    break;
                default:
                    col_brick = "ORANGE";
                    break;
           }
           setColor(brick, col_brick);
           add(window, brick); 
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    // make ball and put it center of window
    int x = (WIDTH / 2) - RADIUS;
    int y = (HEIGHT / 2);
    
    GOval ball = newGOval(x, y, RADIUS * 2, RADIUS * 2);
    setFilled(ball, true);
    setColor(ball, "BLACK");
    add(window, ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    // Make Paddle
    int x_coor = (WIDTH / 2 -(PAD_WIDTH / 2));
    int y_coor = (HEIGHT - 150);
    
    
    GRect paddle = newGRect(x_coor, y_coor, 60, 10);
    setFilled(paddle, true);
    setColor(paddle, "BLACK");
    add(window, paddle);
    
   
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    // make scoreboard
    GLabel score = "0";
    
    score = newGLabel("0");
    setFont(score, "SansSerif-48");
    setColor(score,"LIGHT_GRAY"); 
    double x = (getWidth(window) - getWidth(score)) / 2;
    double y = (getHeight(window) - getHeight(score)) / 2;
    setLocation(score, x, y);
    add(window, score);
    return score;
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
