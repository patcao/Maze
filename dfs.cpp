#include "simulate.h"
#include <iostream>
#include <queue>

//to compile this, use the command "g++ testSim.cpp simulate.cpp -o simulate" in the
//terminal, then run the program with the command "simulate"
//make sure you're in the directory that contains both cpp files and the h file.

using namespace std;

const int dir[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
const int off[4] = {0, 1, 3, 2};

int state[99];                              // known info -1, 0, 1 for don't know, no wall, wall
int vis[99];                                // -1, 0, 1 = no info or cannot visit, viewed as visitable, visited 
int adj0[99];                               // number of adjacent non-walls; used to infer additional non-walls

int where[2];                               // where are we?
int face;                                   // what direction are we facing?
int counter;                                // # of known visitables - # of visited; if it hits 0 we're done        

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
                case 2: cout << "2 "; break;
                default: cout << "? ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void write(int i, int j, int s){
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

void recordInfo(){                          // Check sensors, write information         
    for(int d=0; d<4; d++){
        int rd = (d - face + 4) % 4;

        if(rd == 2) continue;               // we can't see backwards

        if(rd == -1){                        // if we're looking forwards we can see lots :)
            int dw = getFarSensor(rd); 

            for(int w=1; w < dw; w+=2){
                int i = where[0] + w * dir[d][0];
                int j = where[1] + w * dir[d][1];
                write(i, j, 0);

                if(vis[9*(i+dir[d][0])+j+dir[d][1]] == -1){
                    vis[9*(i+dir[d][0])+j+dir[d][1]] = 0;
                    write(i+dir[d][0], j+dir[d][1], 0);
                    counter++;
                } 
            }

            int i = where[0] + dw * dir[d][0];
            int j = where[1] + dw * dir[d][1];
            
            write(i, j, 1);
            write(i + dir[d][1], j + dir[d][0], 1);
            write(i - dir[d][1], j - dir[d][0], 1);
        }

        else{
            int i = where[0] + dir[d][0];
            int j = where[1] + dir[d][1];
            write(i, j, getSensor(rd));

            if(state[9*i+j]){
                write(i + dir[d][1], j + dir[d][0], 1);
                write(i - dir[d][1], j - dir[d][0], 1);
            } 
            else if(!isBoundary(i, j)){
                i += dir[d][0];
                j += dir[d][1];
                if(vis[9*i+j] == -1){
                    vis[9*i+j] = 0;
                    write(i, j, 0); 
                    counter++;
                }
            }
        }
    }
}

struct loc{
    int i, j, k;
    loc (int _i, int _j, int _k){
        i = _i, j = _j, k = _k;
    }
};

void travel(int d){
    if((face+3)%4 == d){
        turnLeft();
        face = d;
        recordInfo();
    }

    while(face != d){
        turnRight();
        face = (face + 1) % 4;
        recordInfo();
    }
    
    forward();
    where[0] += 2 * dir[face][0];
    where[1] += 2 * dir[face][1];
}

void dfs(){
    recordInfo();
    vis[9*where[0]+where[1]] = 1;
    counter--;
    printRep();

    for(bool went = true; went && counter>0;){
        went = false;
    
        for(int i=0; i<4; i++){
            int d = (face + off[i]) % 4;
            int x = where[0] + dir[d][0];
            int y = where[1] + dir[d][1];

            if(state[9*x + y] == 1) continue;
            
            x += dir[d][0], y += dir[d][1];
            if(isBoundary(x, y)) continue;
            if(vis[9*x+y]==1) continue;

            travel(d);
            dfs();
            if(counter == 0) return;
            travel((d+2)%4);
            went = true;
        } 
    }
}

void resetPepe(){
    where[0] = 9;
    where[1] = 7;
    face = 0;
    counter = 1;

    memset(adj0, 0, sizeof(adj0));
    memset(vis, 0xff, sizeof(vis));
    memset(state, 0xff, sizeof(state));

    vis[8 * 9 + 7] = 0;

    for(int row=0; row<11; row++){
        write(row, 0, 1);
        write(row, 8, 1);
    }

    for(int col=0; col<9; col++){
        write(0, col, 1);
        write(10, col, 1);
    }

    write(where[0], where[1], 0);
}

void pepeTheMazeSolver(){
    resetPepe();

    dfs();

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
    
    int times = 1;
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
        }
        printMaze();

        mySleep(1);
    }

    cout << "Avg Moves: " << tot_moves / double(times) << endl;
    cout << "Avg Turns: " << tot_turns / double(times) << endl;

	return 0;	
}

