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


bool isBoundary(int i, int j){
    return i==0 || i==10 || j==0 || j==9;
}

//Prints the algorithm's representation of the maze to stdout
void printRep(){
    for(int i=0; i<11; i++){
        for(int j=0; j<9; j++){
            if(i==where[0] && j==where[1]) cout << "X ";
            else switch(state[9*i+j]){
                case 0: cout << "  "; break;
                case 1: cout << "1 "; break;
                default: cout << "? ";
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


void dfs(int i, int j);
bool visited[99];


void reset(){
    where[0] = 9;
    where[1] = 7;
    face = 0;

    memset(visited, false, sizeof(visited));
    memset(state, 0xff, sizeof(state));

    for(int row=0; row<11; row++){
        state[9 * row + 0] = 1;
        state[9 * row + 8] = 1;
    }

    for(int col=0; col<9; col++){
        state[9 * 0 + col] = 1;
        state[9 * 10 + col] = 1;
    }

    for(int row=1; row<11; row+=2)
        for(int col=1; col<9; col+=2)
            state[9*row + col] = 0;
}

void dfsMazeSolver(){
  reset();
  dfs(9,7);

  // Fix all -1 
  for(int i = 0; i < 99; ++i)
    if(state[i] == -1)
      state[i] = 0;

}

//Turns towards the specified location
void turnTorwards(int i, int j){
  for(int t=0; t<4; ++t){
    int ti = where[0] + 2*dir[ (face+t) % 4][0];
    int tj = where[1] + 2*dir[ (face+t) % 4][1];
    if(ti == i && tj == j){
      switch(t){
        case 0: return;
        case 1: turnRight(); break;
        case 2: turnRight(); turnRight(); break;
        default: turnLeft(); break;
      }
      face = (face + t) % 4;
      return;
    }
  }
}

bool dVisit[99];
bool isMazeDone(){
  memset(dVisit, false, sizeof(dVisit));
  queue< pair<int,int> > q;
  q.push(make_pair(where[0],where[1])); 

  while(!q.empty()){
    pair<int,int> top = q.front();
    q.pop();    
    int curr = 9*top.first + top.second;

    if(state[curr] == -1)
      return false;
    
    dVisit[curr] = true;
    for(int d = 0; d < 4; ++d){
      int di = top.first + dir[ d % 4][0];
      int dj = top.second + dir[ d % 4][1];

      if(!dVisit[9*di+dj] && state[9*di+dj] != 1){
        q.push(make_pair(di,dj));
      }
    }
  }
  cout << "done" << endl;
  return true;
}

int count = 0;
void dfs(int i, int j){
  recordInfo();
  printRep();
  visited[9*i+j] = true;
  state[9*i+j] = 0;
  for(int t = 0; t < 4; ++t){
    if(t == 2)
      continue;
    int ti = i + dir[ (face+t) % 4][0];
    int tj = j + dir[ (face+t) % 4][1];
    if(isBoundary(ti,tj))
      continue;
    if(state[9*ti + tj] != 1 && !visited[9*ti + tj]){
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
      if(isMazeDone())
        return;
      turnTorwards(i,j);
      forward();
      where[0] = i;
      where[1] = j;
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

    int times = 10;

      int tot_moves = 0;
      int tot_turns = 0;
      for(int i = 0; i < times; ++i){
          mazeGen(5);    
          printMaze();         
          dfsMazeSolver();
          printRep();        
          tot_moves += numStepsTaken();
          tot_turns += numTurnsMade();        

         if(!verifyRep()) cout << "pepe was a bad frog" << endl;
          mySleep(1);
      }
      cout << "Avg Moves: " << tot_moves / double(times) << endl;
      cout << "Avg Turns: " << tot_turns / double(times) << endl;
	return 0;	
}
