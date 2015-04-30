#include <iostream>
#include "simulate.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <time.h>
#include <sys/time.h>
#include <fstream>
#include <queue>

using namespace std;

int location[2] = {7,9};

//0 - North   1 - East
//2 - South   4 - West
int direction = 0;
int numSteps = 0;
int numTurns = 0;
int mazeSeed;
const int dir_arr[4] = {-9, 1, 9, -1}; 

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

int getMaze(int n){
  return maze[n];
}

void mazeGen(int thresh){
  struct timeval tp;
  gettimeofday(&tp, NULL);
  mazeSeed = (int) tp.tv_sec * 1000L + tp.tv_usec / 1000;
  srand(mazeSeed);

  //clear maze
  for(int i = 1; i < 10; ++i)
    for(int j = 1; j < 8; ++j)
      maze[i*9 + j] = 0;

  //Lay down vertical section of walls
  for(int i = 0; i+2 < 10; i+=2){
    for(int j = 2; j < 8; j+=2){
      if(rand()%10 >= thresh)
        continue;
      maze[i*9+j] = 1;
      maze[(i+1)*9+j] = 1;
      maze[(i+2)*9+j] = 1;
    }
  }

  //Lay down horizontal section of walls
  for(int i = 2; i < 10; i+=2){
    for(int j = 0; j+2 < 8; j+=2){
      if(rand()%10 >= thresh)
        continue;
      maze[i*9+j] = 1;  
      maze[i*9+j+1] = 1;  
      maze[i*9+j+2] = 1;
    }
  }
  numTurns = 0;
  numSteps = 0;
  location[0] = 7;
  location[1] = 9;
  direction = 0;
  clearSections(); 
}

void mazeWithSeed(int seed){
  mazeSeed = seed;
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
  clearSections(); 
  numTurns = 0;
  numSteps = 0;
}

void clearSections(){
  bool v[99];
  memset(v,false,sizeof(v));

  int loc1D = 9*9 + 7;
  queue<int> q;
  q.push(loc1D);
  //mark edges as seen
  for(int i = 0; i < 99; i+=9)
    v[i] = true, v[i+8] = true;
  for(int j = 0; j < 9; ++j)
    v[j] = true, v[10*9+j] = true;

  //bfs
  while(!q.empty()){
    int curr = q.front();
    q.pop();
    v[curr] = true;

    for(int i = 0; i < 4; ++i){
      int next = curr + dir_arr[i];
      if(!v[next] && !maze[next])
        q.push(next);

      if(!v[next] && maze[next])
        v[next] = true;
    }

    //fixes up cut corners
    int diag[4] = {-8,-10,8,10};
    for(int i = 0; i < 4; ++i){
      int next = curr + diag[i];
      if(!v[next] && maze[next])
        v[next] = true;
    }
      
  }

  for(int i = 0; i < 99; ++i)
    if(!v[i])
      maze[i] = 0;
}

bool forward() {  
	int loc1D = location[1]*9 + location[0];
  //update robot location  
	if (direction == 0) 
		location[1]-= 2;	
	else if (direction == 1) 
		location[0] += 2;
	else if (direction == 2) 
		location[1] += 2;	
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
  direction = (direction + 3) % 4;
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

int getFarSensor(int dir){
	int loc1D = location[1]*9 + location[0];
  int ret = 0;
  for(; loc1D > 0 && loc1D < 99; loc1D += dir_arr[ (direction + dir) % 4]){
    if(maze[loc1D])
      return ret;
      ++ret;
  }
  return ret;
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
  return maze[loc1D + dir_arr[ (direction+1)%4]];
}

int getSensorLeft() {
	int loc1D = location[1]*9 + location[0];
  return maze[loc1D + dir_arr[ (direction+3)%4]];
}

int getSensorBehind() {
	int loc1D = location[1]*9 + location[0];
  return maze[loc1D + dir_arr[ (direction+2)%4]];
}


int getDirection(){
  return direction;
}

std::pair<int,int> getLocation(){
  return make_pair(location[1],location[0]);
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

int numTurnsMade(){
    return numTurns;
}

void printStats(){
  cout << "Steps: " << numSteps << endl;
  cout << "Turns: " << numTurns << endl;
}

void mySleep(int ms){
  struct timeval tp;
  gettimeofday(&tp, NULL);
  long long curr = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;
  long long end = curr + ms;
  while(curr < end){
    gettimeofday(&tp, NULL);
    curr = (long long) tp.tv_sec * 1000L + tp.tv_usec / 1000;
  }

}
