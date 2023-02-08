#ifndef MAINAPP_H
#define MAINAPP_H

#include <iostream>
#include <utility>  
#include <vector>
#include <fstream>
#include <string>

using namespace std;

enum cellState {USED,NOTEXIST,UNUSED};

struct Container{
    string name;
    string description;
    pair<int,int> XY;
};

struct Cell{
    cellState status;
    pair<int,int> XY;
    Container *container;
};


class mainApp{
    private:
        Cell* ship[8][12];
        Cell* buffer[4][24];
        int currMove;
        vector<int> moveOrder;
        string currShipName;
        string currOpName;
        int estimateTimeInMin;

        ifstream manifest;

        void writeToLog(string);
        void parseManifest();
        int calculateTime(pair<int,int>, pair<int,int>);



    public:
        mainApp();
        ~mainApp();
        void newShip(ifstream);
        string getShipName();
        string getOpName();
        int getNextMove();
        int getEstimatedTimeInMin();
        void unload_load();
        void balance();
        void containerToBeLoaded(int, string);
        void opComments(string);

};

#endif