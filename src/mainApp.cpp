#include "header/mainApp.h"
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

void mainApp::parseManifest(){

}



int main(){
    return 0;
}
