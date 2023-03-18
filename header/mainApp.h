#ifndef MAINAPP_H
#define MAINAPP_H

#include <iostream>
#include <utility>  
#include <vector>
#include <fstream>
#include <string>
#include <unordered_map>
#include <queue>
#include <stack>
#include <chrono>
#include <ctime> 
#include <iomanip>
#include <shlobj.h>

using namespace std;

enum cellState {USED,NOTEXIST,UNUSED};
enum CraneState {SHIP,TRUCK,BUFFER};



struct Container{
    int weight = -1;
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


struct State{
    Cell ship[8][12];
    stack<Container> buffer;
    vector<Container> toBeUnloaded;
    vector<Container> toBeLoaded;
    int time = 0;
    int cost = 0;
    int depth = 0;
    int balanceDifference = 99999;
    //int selectedContWeight = 0;
    //int misplacedTiles = 0;
    int estRemainingCost = 9999999;
    //float balance = 0;
    pair<int,int> craneLocation;
    

    // pair.first is the avaialbe row in the column
    // pair.second is the number of containers column has
    pair<int,int> numOfcontainerInColumn[12];
    
    unordered_map<string, vector<Container>> hashMapForContainer;
    vector<vector<pair<int,int>>> containerMoveOrder;

    CraneState craneState;

};

class Compare{
    public:
        bool operator()(State &a, State &b){
             return a.cost > b.cost;
        }
};

class balanceCompare{
    public:
        bool operator()(State &a, State &b){
           
            return (a.cost + a.estRemainingCost > b.cost + b.estRemainingCost) || (a.balanceDifference > b.balanceDifference); //best heuristic for now, somehow????

        }
};


class mainApp{
    private:
        State solutionState;
        string currShipName;
        string currOpName;
        string manifestName;
        
        int estimateTimeInMin;

        priority_queue <State, vector<State>, Compare> bestState;
        priority_queue <State, vector<State>, balanceCompare> bestStateBalance;
        vector<State> duplicateNodes;

        Container noContainer;

        ifstream manifest;

        pair<int,int> pinkCell;
        pair<int,int> pinkCellBuffer;

        void writeToLog(string);
        void parseManifest();
        
        
        int calculateTime(pair<int,int>&, pair<int,int>&);
        void updateNumContainerAbove(int, int, State&);
        
        Container getContainerWithKey(string&,State&);
        bool moveContainer(int column, Container&, State&, int);
        pair<int,int> findHighestColumnBetween(pair<int,int>&, pair<int,int>&, State&);

        void initNewState();

        void unload_one(State&);
        void load_one(State&);
        void moveToBuffer(State&, Container);
        int calculateEmptyColumn(State&, int);
        void addMoveOrder(State&, pair<int,int>,pair<int,int>,pair<int,int>);
        void createManifest();
        void removeContainer(Container& container, State&);

        float calculateLeftSideWeight(State&);
        float calculateRightSideWeight(State&);
        float calculateBalance(State&);
        stack<Container> balanceList(State&);
        void balance_one(State&, Container);
        bool moveContainerBalance(int column, Container&, State&, int);
        void moveToBufferBalance(State&, Container);
        bool siftCheck();
        State siftProcedure();
        int calcMisplaced(State&);

        int ROW_SHIP = 8;
        int COLUMN_SHIP = 12;

        int ROW_BUFFER = 4;
        int COLUMN_BUFFER = 24; 

        int TIME_FROM_SHIP_TO_TRUCK = 2;
        int TIME_FROM_SHIP_TO_BUFFER = 4;

        int NOT_EXIST = -1;

        int currMoveSequence = 0;

    public:
        State initState;
        mainApp();
        void newShip(string manifest);
        string getShipName();
        string getOpName();
        vector<pair<int,int>> getNextMoveSequence();
        int getEstimatedTimeInMin();
        State unload_load(vector<string>&, vector<Container>&);
        void balance();
        void addComments(string);
        int numOfMovesRemain();
        

        Container getContainer(int, int);
        
        State balanceSearch();
        void addComments(string);
        

        Container getContainer(int, int);



};

#endif
