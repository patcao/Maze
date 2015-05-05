#include "simulate.h"
#include <iostream>
#include <queue>
#include <time.h>
#include <sys/time.h>

//to compile this, use the command "g++ testSim.cpp simulate.cpp -o simulate" in the
//terminal, then run the program with the command "simulate"
//make sure you're in the directory that contains both cpp files and the h file.

using namespace std;

const char INF = 0x7f;
const char dir[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};

char state[99];                              // -1, 0, 1 for don't know, no wall, wall
int where[2] = {9, 7};
char adj0[99];
int face = 0;            // where are we and what direction are we facing?
char dst[11][9][4];

struct loc{
    int i, j, k;
    loc (int _i, int _j, int _k){
        i = _i, j = _j, k = _k;
    }
};


bool isBoundary(int i, int j){
    return i==0 || i==10 || j==0 || j==9;
}

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

void write_loc(int i, int j, int s){
    int idx = 9 * i + j;
    if(state[idx] != -1) return;
    
    state[idx] = s;
    
    if(s == 0){
        for(int k=0; k<4; k++){
            int nbr = 9 * (i + dir[k][0]) + j + dir[k][1];
            if(++adj0[nbr] == 4 && state[nbr] == -1) state[nbr] = 0;
        }
    }
}

void recordInfo(){                          // Check sensors, write_loc information         
    for(int d=0; d<4; d++){
        int rd = (d - face + 4) % 4;

        if(rd == 2) continue;               // we can't see backwards

        if(rd == 0){                        // if we're looking forwards we can see lots :)
            int dw = getFarSensor(rd); 

            for(int w=1; w < dw; w+=2){
                write_loc(where[0] + w*dir[d][0], where[1] + w*dir[d][1], 0);
                write_loc(where[0] + (w+1)*dir[d][0], where[1] + (w+1)*dir[d][1], 0); 
            }

            int i = where[0] + dw * dir[d][0];
            int j = where[1] + dw * dir[d][1];
            
            write_loc(i, j, 1);
            write_loc(i + dir[d][1], j + dir[d][0], 1);
            write_loc(i - dir[d][1], j - dir[d][0], 1);
        }

        else{
            int i = where[0] + dir[d][0];
            int j = where[1] + dir[d][1];
            write_loc(i, j, getSensor(rd));

            if(state[9*i+j]){
                write_loc(i + dir[d][1], j + dir[d][0], 1);
                write_loc(i - dir[d][1], j - dir[d][0], 1);
            }
            else
                write_loc(i + dir[d][0], j + dir[d][1], 0);
        }
    }
}

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

int numTimes = 0;
long long seesTime = 0;
bool seesNew(int i, int j, int f){
    struct timeval before,after;
    gettimeofday(&before, NULL);

    ++numTimes;
    for(int d=0; d<4; d++){
        int rd = (f + d) % 4;

        if(d == 2) continue;       // we can't see backwards

        if(d == 0){                // we can see forwards a lot
            for(int w=1; ; w+=2){
                int see = state[9*(i+w*dir[rd][0]) + j+w*dir[rd][1]];
                if(see == 1) break;
                if(see == -1) return true;
            } 
        }
    
        else if(state[9*(i+dir[rd][0]) + j+dir[rd][1]] == -1) return true;
    }

    gettimeofday(&after, NULL);
    seesTime += ((after.tv_sec - before.tv_sec)*1000000L + after.tv_usec) - before.tv_usec;
    return false;
}

long long bfsTime = 0;
bool findNearestUnknown(){
    memset(dst, 0x7f, sizeof(dst));
    queue<loc> q;
    q.push(loc(where[0], where[1], face));
    dst[where[0]][where[1]][face] = 0;

    struct timeval before,after;
    gettimeofday(&before, NULL);
    while(!q.empty()){
        loc top = q.front();
        q.pop();
        
        if(seesNew(top.i, top.j, top.k)){

            gettimeofday(&after, NULL);
            bfsTime += ((after.tv_sec - before.tv_sec)*1000000L + after.tv_usec) - before.tv_usec;
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

void resetPepe(){
    where[0] = 9;
    where[1] = 7;
    face = 0;

    memset(adj0, 0, sizeof(adj0));
    memset(state, 0xff, sizeof(state));

    for(int row=0; row<11; row++){
        write_loc(row, 0, 1);
        write_loc(row, 8, 1);
    }

    for(int col=0; col<9; col++){
        write_loc(0, col, 1);
        write_loc(10, col, 1);
    }

    write_loc(where[0], where[1], 0);
}

long long totalTime = 0;
long long totalTime2 = 0;
void pepeTheMazeSolver(){
    resetPepe();

    struct timeval before,after;
    gettimeofday(&before, NULL);

    do{
        recordInfo();
        //printRep();
    } while(findNearestUnknown());
    gettimeofday(&after, NULL);

    totalTime += ((after.tv_sec - before.tv_sec)*1000000L + after.tv_usec) - before.tv_usec;
    for(int i = 0; i < 99; ++i)
        if(state[i] == -1)
            state[i] = 2;
    
    //printRep();
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
    int times = 10000;
    int tot_moves = 0;
    int tot_turns = 0;

    for(int i = 0; i < times; ++i){
        mazeGen(5);    
    
        pepeTheMazeSolver();
       
        tot_moves += numStepsTaken();
        tot_turns += numTurnsMade();        

        //printStats();
        if(!verifyRep()) {
            cout << "pepe was a bad frog" << endl;
            printRep();
        }
        mySleep(1);
    }

    cout << "bfs time " << bfsTime << endl;
    cout << "seesNew time " << seesTime << endl;
    cout << "seesNew " << numTimes / double(times) << endl;
    cout << "Total Time: " << totalTime << endl;
    cout << "Avg Moves: " << tot_moves / double(times) << endl;
    cout << "Avg Turns: " << tot_turns / double(times) << endl;

	return 0;	
}

