#include "../header/mainApp.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

// constructor
mainApp :: mainApp() {
    
    // set all buffer to UNUSED
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 24; j++){
            Cell *temp = new Cell();
            temp->status = UNUSED;
            temp->XY.first = i;
            temp->XY.second = j;
            
            buffer[i][j] = temp;
        }
    }
}

mainApp :: ~mainApp(){
    
    // free buffer
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 24; j++){
        
            delete buffer[i][j];
        }
    }
}

string mainApp::getShipName(){
    return this->currShipName;
}

string mainApp::getOpName() {
    return this->currOpName;
}

void mainApp::newShip(string manifest){
    this-> manifestName = manifest;
    parseManifest();
}

void mainApp::parseManifest(){
    manifest.open(manifestName);

    if(!manifest.is_open()){
        cout << "File not open" << endl;
        return;
    }
    string currLine;
    
    // parse manifest
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 12; j++){
            getline(manifest, currLine);
        
            // string x = currLine.substr(1,2);
            // string y = currLine.substr(4,2);

            string weight = currLine.substr(10,5);

            string description = currLine.substr(18,currLine.size()-1);
            
            if(description == "NAN"){
                Cell *tempCell = new Cell();
                tempCell->status = NOTEXIST;
                tempCell->XY.first = i;
                tempCell->XY.second = j;
                tempCell->container = nullptr;

                ship[i][j] = tempCell;
            }

            if(description == "UNUSED"){
                Cell *tempCell = new Cell();
                tempCell->status = UNUSED;
                tempCell->XY.first = i;
                tempCell->XY.second = j;
                tempCell->container = nullptr;

                ship[i][j] = tempCell;
            }

            else{
                Cell *tempCell = new Cell();
                tempCell->status = USED;
                tempCell->XY.first = i;
                tempCell->XY.second = j;

                Container* container = new Container();
                container->description = description;
                container->weight = stoi(weight);
                container->XY.first = i;
                container->XY.second = j;
                
                tempCell->container = container;

                ship[i][j] = tempCell;
            }
        }
    }

}

Container* mainApp::getContainer(int x, int y){
    return ship[x][y]->container;
}


