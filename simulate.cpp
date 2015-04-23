#include <iostream>
#include "simulate.h"
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <fstream>

using namespace std;

int location[2] = {7,9};
int direction = 0;
int numSteps = 0;
int numMoves = 0;
time_t mazeSeed;

//the maze is 9x11 to represent the physical 4'x5' maze.
int maze[99] = {  1, 1, 1, 1, 1, 1, 1, 1, 1,    
              		1, 0, 0, 0, 1, 0, 0, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 0, 0, 1, 0, 1,
          				1, 1, 1, 1, 1, 1, 1, 1, 1 };

void mazeGen(){
  mazeSeed = time(NULL);
  srand(mazeSeed);

  //clear maze
  for(int i = 1; i < 10; ++i)
    for(int j = 1; j < 8; ++j)
      maze[i*9 + j] = 0;

  //Lay down vertical section of walls
  for(int i = 0; i+2 < 10; i+=2){
    for(int j = 2; j < 8; j+=2){
      if(rand()%2)
        continue;
      maze[i*9+j] = 1;
      maze[(i+1)*9+j] = 1;
      maze[(i+2)*9+j] = 1;
    }
  }

  //Lay down horizontal section of walls
  for(int i = 2; i < 10; i+=2){
    for(int j = 0; j+2 < 8; j+=2){
      if(rand()%2)
        continue;
      maze[i*9+j] = 1;  
      maze[i*9+j+1] = 1;  
      maze[i*9+j+2] = 1;
    }
  }

}

bool forward() {  
	int location1D = location[1]*9 + location[0];
	bool hitWall = false;
	if (direction == 0) {
		if (maze[location1D-9] == 1)
			hitWall = true;
		location[1]-= 2;
	}
	else if (direction == 1) {
		if (maze[location1D+9] == 1)
			hitWall = true;
		location[1] += 2;
	}
	else if (direction == 2) {
		if (maze[location1D+1] == 1)
			hitWall = true;
		location[0] += 2;
	}
	else {
		if (maze[location1D-1] == 1)
			hitWall = true;
		location[0] -= 2;
	}
	if (hitWall) {
		cout << "HIT WALL!" << endl;
		return false;
	}
	if (location[0] > 8 || location[0] < 0 || location[1] > 10 || location[1] < 0)
		return false;

  ++numSteps;
  ++numMoves;
	return true;
}

bool turnRight() {
	direction = (direction == 0 ? 2 : (
					direction == 1 ? 3 : (
					direction == 2 ? 1 : 0)));
  ++numMoves;
	return true;
}

bool turnLeft() {
	direction = (direction == 0 ? 3 : (
					direction == 1 ? 2 : (
					direction == 2 ? 0 : 1)));
  ++numMoves;
	return true;
}

int getSensor(int dir){
  switch(dir){
    case 0: return getSensorFront();
    case 1: return getSensorRight();
    case 2: return getSensorBehind();
    default: return getSensorLeft();
  }
}
int getSensorFront() {
	int location1D = location[1]*9 + location[0];
	if (direction == 0)
		return maze[location1D-9];
	else if (direction == 1)
		return maze[location1D+9];
	else if (direction == 2)
		return maze[location1D+1];
	else
		return maze[location1D-1];
}

int getSensorRight() {
	int location1D = location[1]*9 + location[0];
	if (direction == 0)
		return maze[location1D+1];
	else if (direction == 1)
		return maze[location1D-1];
	else if (direction == 2)
		return maze[location1D+9];
	else
		return maze[location1D-9];
}

int getSensorLeft() {
	int location1D = location[1]*9 + location[0];
	if (direction == 0)
		return maze[location1D-1];
	else if (direction == 1)
		return maze[location1D+1];
	else if (direction == 2)
		return maze[location1D-9];
	else
		return maze[location1D+9];}

int getSensorBehind() {
	int location1D = location[1]*9 + location[0];
	if (direction == 0)
		return maze[location1D+9];
	else if (direction == 1)
		return maze[location1D-9];
	else if (direction == 2)
		return maze[location1D-1];
	else
		return maze[location1D+1];
}


void printCurrPos(){
  for(int i = 0; i < 11; ++i){
    for(int j = 0; j < 9; ++j){
      if(i==location[1] && j==location[0]){
        cout << "X ";
        continue;
      }
      if(maze[9*i+j])
        cout << 1 << " ";
      else
        cout << "  ";
    }
    cout << endl;
  }
  cout << endl;
}

void printLocation() { //for debugging only
	cout << "x = " << location[0] << " y = " << location[1] << endl;
	return;
}

void printDirection() { //for debugging only
	cout << "facing ";
	if (direction == 0)
		cout << "north";
	else if (direction == 1)
		cout << "south";
	else if (direction == 2)
		cout << "east";
	else
		cout << "west";
	cout << endl;
	return;
}


void printMaze(){
  cout << endl << "Maze Id: " << mazeSeed << endl;
  for(int i = 0; i < 11; ++i){
    for(int j = 0; j < 9; ++j){
      if(maze[9*i+j])
        cout << 1 << " ";
      else
        cout << "  ";
    }
    cout << endl;
  }
  cout << endl;

}

int numStepsTaken(){
  return numSteps;
}

void printStats(){
  cout << "Steps: " << numSteps << endl;
  cout << "Moves: " << numMoves << endl;

}

