/* THIS CODE DESIGNED FOR ECE 3400 STUDENTS ONLY.
   IT MAY NOT BE REPRODUCED OR DISTRIBUTED WITHOUT
   PERMISSION OF STAFF. */
#include <utility>
    
#ifndef SIMULATE
#define SIMULATE

int getMaze(int n);
//Give a thresh in the range [0,10]
//Specifies how likely a wall is to be placed
void mazeGen(int thresh);
void mazeWithSeed(int seed);
void clearSections();
bool forward();
bool turnRight();
bool turnLeft();

int getSensorFront();
int getSensorRight();
int getSensorLeft();
int getSensorBehind();
int getSensor(int dir);
int getFarSensor(int d);
int getDirection();
std::pair<int,int> getLocation();

void printCurrPos();
void printLocation();
void printDirection();
void printMaze();

int numStepsTaken();
int numTurnsMade();
void printStats();
void mySleep(int ms);
#endif
