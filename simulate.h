/* THIS CODE DESIGNED FOR ECE 3400 STUDENTS ONLY.
   IT MAY NOT BE REPRODUCED OR DISTRIBUTED WITHOUT
   PERMISSION OF STAFF. */
   
#ifndef SIMULATE
#define SIMULATE

void mazeGen();
bool forward();
bool turnRight();
bool turnLeft();

int getSensorFront();
int getSensorRight();
int getSensorLeft();
int getSensorBehind();
int getSensor(int dir);
int getFarSensor(int d);

void printCurrPos();
void printLocation();
void printDirection();
void printMaze();

int numStepsTaken();
void printStats();

#endif
