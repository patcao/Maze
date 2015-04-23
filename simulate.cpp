#include <iostream>
#include "simulate.h"
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <fstream>

using namespace std;

int location[2] = {7,9};
//0 - North   1 - East
//2 - South   4 - West
int direction = 0;
int numSteps = 0;
int numTurns = 0;
time_t mazeSeed;
const int dir_arr[4] = {-9, 1, 9,- 1}; 

//the maze is 9x11 to represent the physical 4'x5' maze.
int maze[99] = {  1, 1, 1, 1, 1, 1, 1, 1, 1,    
              		1, 0, 0, 0, 1, 0, 0, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 0, 1, 0, 1,
          				1, 0, 1, 0, 1, 1, 0, 0, 1,
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
  
  numTurns = 0;
  numSteps = 0;
}

bool forward() {  
	int loc1D = location[1]*9 + location[0];
  /*
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
  */

  
  //update robot location  
	if (direction == 0) 
		location[1]-= 2;	
	else if (direction == 1) 
		location[1] += 2;
	else if (direction == 2) 
		location[0] += 2;	
	else 
		location[0] -= 2;
	

	if (maze[loc1D + dir_arr[direction]]) {
		cout << "HIT WALL!" << endl;
		return false;
	}
	if (location[0] > 8 || location[0] < 0 || location[1] > 10 || location[1] < 0)
		return false;

  ++numSteps;
	return true;
}

bool turnRight() {
  direction = (direction + 1) % 4;
  ++numTurns;
	return true;
}

bool turnLeft() {
  direction = (direction - 1) % 4;
  ++numTurns;
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
	int loc1D = location[1]*9 + location[0];
  /*
  int ret = 0;
  for(; loc1D > 0 && loc1D < 99; loc1D += dir_arr[direction]){
    if(maze[loc1D])
      return ret;
    ++ret;
  }
  return ret;*/
  return maze[loc1D + dir_arr[direction]];
}

int getSensorRight() {
	int loc1D = location[1]*9 + location[0];
  /*
  int ret = 0;
  for(; loc1D > 0 && loc1D < 99; loc1D += dir_arr[ (direction+1) % 4]){
    if(maze[loc1D])
      return ret;
    ++ret;
  }
  return ret;
  */
  return maze[loc1D + dir_arr[ (direction+1)%4]];
}

int getSensorLeft() {
	int loc1D = location[1]*9 + location[0];
  /*
	if (direction == 0)
		return maze[location1D-1];
	else if (direction == 1)
		return maze[location1D+1];
	else if (direction == 2)
		return maze[location1D-9];
	else
		return maze[location1D+9];
    */
  return maze[loc1D + dir_arr[ (direction+3)%4]];
}

int getSensorBehind() {
	int loc1D = location[1]*9 + location[0];
  /*
	if (direction == 0)
		return maze[location1D+9];
	else if (direction == 1)
		return maze[location1D-9];
	else if (direction == 2)
		return maze[location1D-1];
	else
		return maze[location1D+1];
    */
  return maze[loc1D + dir_arr[ (direction+2)%4]];
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
  switch(direction){
    case 0: cout << "north"; break;
    case 1: cout << "east"; break;
    case 2: cout << "south"; break;
    default: cout << "west"; break;
  }
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
  cout << "Turns: " << numTurns << endl;
}
