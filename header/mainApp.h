#ifndef MAINAPP_H
#define MAINAPP_H

#include <iostream>
#include <utility>  
#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>

using namespace std;

enum cellState {USED,NOTEXIST,UNUSED};
enum CraneState {SHIP,TRUCK,BUFFER};

int ROW_SHIP = 8;
int COLUMN_SHIP = 12;

int ROW_BUFFER = 4;
int COLUMN_BUFFER = 24; 

int TIME_FROM_SHIP_TO_TRUCK = 2;
int TIME_FROM_SHIP_TO_BUFFER = 4;

int NOT_EXIST = -1;

struct State{
    Cell ship[8][12];
    Cell buffer[4][24];
    vector<Container> toBeUnloaded;
    vector<Container> toBeLoaded;
    int time = 0;
    int cost = 0;
    pair<int,int> craneLocation;
    

    // pair.first is the avaialbe row in the column
    // pair.second is the number of containers column has
    pair<int,int> numOfcontainerInColumn[12];
    
    unordered_map<string, vector<Container>> hashMapForContainer;
    vector<vector<pair<int,int>>> containerMoveOrder;

    CraneState craneState;



};

struct Container{
    int weight;
    string description;
    pair<int,int> XY;
    string key;
    int numContainerAbove;
};

struct Cell{
    cellState status;
    pair<int,int> XY;
    Container container;
};

class Compare{
    public:
        bool operator()(State &a, State &b){
            return a.cost < b.cost;
        }
};


class mainApp{
    private:
        State initState;
        string currShipName;
        string currOpName;
        string manifestName;
        
        int estimateTimeInMin;

        priority_queue <State, vector<State>, Compare> bestState;

        Container noContainer;

        ifstream manifest;

        pair<int,int> pinkCell;

        void writeToLog(string);
        void parseManifest();
        
        
        int calculateTime(pair<int,int>, pair<int,int>);
        void calculateNumContainerAbove(int, State&);
        void updateNumContainerAbove(int, int, State&);
        
        Container getContainerWithKey(string,State&);
        void moveContainer(int column, Container, State, State&, int);
        pair<int,int> findHighestColumnBetween(pair<int,int>, pair<int,int>, State&);

        void initNewState();

        void unload_one(State);
        void load_one(State);
        void moveToBuffer(State&, Container );

    public:
        mainApp();
        void newShip(string manifest);
        string getShipName();
        string getOpName();
        int getNextMove();
        int getEstimatedTimeInMin();
        State unload_load(vector<string>&, vector<Container>&);
        void balance();
        void opComments(string);

        Container getContainer(int, int);

};

#endif