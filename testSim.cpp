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

struct loc{
    int i, j, k;
    loc (int _i, int _j, int _k){
        i = _i, j = _j, k = _k;
    }
};

int dst[11][9][4];

void go(int i, int j, int k){
    if(i == where[0] && j == where[1] && k == face) return;

    for(int t=1; t<=3; t+=2){
        if(dst[i][j][(k+t)%4] < dst[i][j][k]){
            go(i, j, (k+t) % 4);
            if(t==1) turnLeft();
            else turnRight();
            face = (face + 4 - t) % 4;
            return;
        }
    }

    go(i-2*dir[k][0], j-2*dir[k][1], k);
    forward();
    where[0] += 2*dir[k][0];
    where[1] += 2*dir[k][1];
}

bool seesNew(int i, int j){
    for(int d=0; d<4; d++)
        if(state[9*(i+dir[d][0]) + j+dir[d][1]] == -1)
            return true;
    return false;
}

bool findNearestUnknown(){
    memset(dst, 0x7f, sizeof(dst));
    queue<loc> q;
    q.push(loc(where[0], where[1], face));
    dst[where[0]][where[1]][face] = 0;

    while(!q.empty()){
        loc top = q.front();
        q.pop();
        
        if(seesNew(top.i, top.j)){
            go(top.i, top.j, top.k);
            return true;
        }

        for(int t=1; t<=3; t+=2)
            if(dst[top.i][top.j][(top.k+t)%4] == INF){
                dst[top.i][top.j][(top.k+t)%4] = dst[top.i][top.j][top.k] + 1;
                q.push(loc(top.i, top.j, (top.k+t)%4));
            }

        int facing = 9 * (top.i + dir[top.k][0]) + top.j + dir[top.k][1];
        if(facing >= 0 && facing<=99 && state[facing] != 1){
            int fi = top.i + 2 * dir[top.k][0];
            int fj = top.j + 2 * dir[top.k][1];

            if(fi>=0 && fi<11 && fj>=0 && fj<9 && dst[fi][fj][top.k] == INF){
                dst[fi][fj][top.k] = dst[top.i][top.j][top.k] + 1;
                q.push(loc(fi, fj, top.k));
            }
        }
    }

    return false;
}

void pepeTheMazeSolver(){
    memset(state, 0xff, sizeof(state));
    do{
        recordInfo();
        //printRep();
    }while(findNearestUnknown());

  //mark the edge of the maze
  for(int i = 0; i < 99; i+=9)
    state[i] = 1, state[i+8] = 1;
  for(int j = 0; j < 9; ++j)
    state[j] = 1, state[10*9+j] = 1;

  // Fix all -1 
  for(int i = 0; i < 99; ++i)
    if(state[i] == -1)
      state[i] = 0;

  printRep();
}

bool verifyRep(){
  for(int i = 0; i < 99; ++i)
    if(getMaze(i) != state[i]){
      cout << i << " " << getMaze(i) << " " << state[i] << endl;
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
    pepeTheMazeSolver();
    printStats();
    cout << verifyRep() << endl;
    mySleep(1);
}
   
	return 0;	
}

