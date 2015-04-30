#include "simulate.h"
#include <iostream>
#include <queue>

//to compile this, use the command "g++ testSim.cpp simulate.cpp -o simulate" in the
//terminal, then run the program with the command "simulate"
//make sure you're in the directory that contains both cpp files and the h file.

using namespace std;

const int INF = 0x7f7f7f7f;
const int dir[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

int state[99];                              // -1, 0, 1 for don't know, no wall, wall
int where[2] = {9, 7};

int face = 0;            // where are we and what direction are we facing?

struct loc{
    int i, j, k;
    loc (int _i, int _j, int _k){
        i = _i, j = _j, k = _k;
    }
};


void dfs(int i, int j);


bool isBoundary(int i, int j){
    return i==0 || i==10 || j==0 || j==9;
}

void printRep(){
    for(int i=0; i<11; i++){
        for(int j=0; j<9; j++){
            if(i==where[0] && j==where[1]) cout << "X ";
            else if(i%2 && j%2) cout << "  ";
            else switch(state[9*i+j]){
                case 0: cout << "  "; break;
                case 1: cout << "1 "; break;
                default: cout << (isBoundary(i, j) ? "1 " : "  ");
            }
        }
        cout << endl;
    }
    cout << endl;
}

void recordInfo(){                          // Check sensors, write information         
    for(int d=0; d<4; d++){
        int i = where[0] + dir[d][0];
        int j = where[1] + dir[d][1];
        state[9*i+j] = getSensor((d - face + 4)%4);

        if(state[9*i+j]){
            state[9*(i+dir[d][1]) + j+dir[d][0]] = true;
            state[9*(i-dir[d][1]) + j-dir[d][0]] = true;
        }
    }
}


bool seesNew(int i, int j){
    for(int d=0; d<4; d++)
        if(state[9*(i+dir[d][0]) + j+dir[d][1]] == -1)
            return true;
    return false;
}



bool visited[99];

void dfsMazeSolver(){
    memset(state, 0xff, sizeof(state));
    memset(visited,false, sizeof(visited));
    where[0] = 9;
    where[1] = 7;
    face = 0;

  //mark the edge of the maze
  for(int i = 0; i < 99; i+=9)
    state[i] = 1, state[i+8] = 1;
  for(int j = 0; j < 9; ++j)
    state[j] = 1, state[10*9+j] = 1;

  dfs(9,7);

  // Fix all -1 
  for(int i = 0; i < 99; ++i)
    if(state[i] == -1)
      state[i] = 0;

}

int count = 0;
void dfs(int i, int j){
  recordInfo();
  printRep();

  visited[9*i+j] = true;
  for(int t = 0; t < 4; ++t){
    if(t == 2)
      continue;
    int ti = i + dir[ (face+t) % 4][0];
    int tj = j + dir[ (face+t) % 4][1];

    if(isBoundary(ti,tj))
      continue;

    if(state[9*ti + tj] != 1 && !state[9*ti + tj]){
      visited[9*ti+tj] = true;

      int di = i + 2*dir[(face+t)%4][0];      
      int dj = j + 2*dir[(face+t)%4][1];
      switch(t){
        case 0: break;
        case 1: turnRight(); face=(face+1)%4;  break;
        default:  turnLeft(); face = (face+3)%4; break;
      }
      forward();
      where[0] = di;
      where[1] = dj;
      dfs(di,dj);
    }
  }
}

bool verifyRep(){
  for(int i = 0; i < 99; ++i)
    if(getMaze(i) != state[i]){
      cout << i << " " << getMaze(i) << " " << state[i] << endl;
      return false;
    }
  if(getDirection() != face){
    cout << "Direction: " << getDirection() << "Robo: " << face << endl;
    return false;
  }

  pair<int,int> l = getLocation();  
  if(l.first != where[0] || l.second != where[1]){
    cout << "Loc: " << where[0] << " " << where[1] << " ";
    cout << l.first << " " << l.second << endl;
    return false;
  }
  
  return true;
}


int main() { 
  /*
  printLocation();
  for(int i = 0; i < 4; ++i){
    printDirection();
    cout <<  "Front: " << getFarSensor(0) << endl;
    cout <<  "Right: " <<  getFarSensor(1) << endl;
    cout <<  "Behind: " <<  getFarSensor(2) << endl;
    cout <<  "Left: " << getFarSensor(3) << endl << endl;  
    turnLeft();
  }
  */
  int times = 1;
  for(int i = 0; i < times; ++i){
    mazeGen();    
    //mazeWithSeed(-298599628);
    printMaze();   
    dfsMazeSolver();
    printStats();
    cout << verifyRep() << endl;
    mySleep(1);
  }
   
	return 0;	
}

