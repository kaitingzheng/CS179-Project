#include "../header/mainApp.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

struct greater_than_key
{
    inline bool operator() (const Container* struct1, const Container* struct2)
    {
        return (struct1->numContainerAbove > struct2->numContainerAbove);
    }
};

// constructor
mainApp :: mainApp() {
    pinkCell.first = ROW_SHIP + 1;
    pinkCell.second = -1;
}

string mainApp::getShipName(){
    return this->currShipName;
}

string mainApp::getOpName() {
    return this->currOpName;
}

void mainApp::initNewState(){

    State newInitState;
    initState = newInitState;
    
    // set all buffer to UNUSED
    for(int i = 0; i < ROW_BUFFER; i++){
        for(int j = 0; j < COLUMN_BUFFER; j++){
            Cell temp;
            temp.status = UNUSED;
            temp.XY.first = i;
            temp.XY.second = j;
            
            initState.buffer[i][j] = temp;
        }
    }

    for(int i = 0; i < COLUMN_SHIP; i++){
        pair<int,int> temp;
        temp.first = NOT_EXIST;
        temp.second = 0; 
        initState.numOfcontainerInColumn[i] = temp;
    }

    initState.craneLocation = pinkCell;
    initState.craneState = SHIP;
}

void mainApp::newShip(string manifest){
    initNewState();
    this-> manifestName = manifest;
    parseManifest();

    noContainer.weight = -1;
    initState.craneLocation.first = -1;
    initState.craneLocation.second = -1;
    initState.cost = 0;
    initState.time = 0;
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
                Cell tempCell;
                tempCell.status = NOTEXIST;
                tempCell.XY.first = i;
                tempCell.XY.second = j;

                initState.ship[i][j] = tempCell;
            }

            if(description == "UNUSED"){
                Cell tempCell;
                tempCell.status = UNUSED;
                tempCell.XY.first = i;
                tempCell.XY.second = j;

                initState.ship[i][j] = tempCell;

                if(initState.numOfcontainerInColumn[j].first == NOT_EXIST){
                    initState.numOfcontainerInColumn[j].first = i;
                }


            }

            else{
                Cell tempCell;
                tempCell.status = USED;
                tempCell.XY.first = i;
                tempCell.XY.second = j;

                Container container;
                container.description = description;
                container.weight = stoi(weight);
                container.XY.first = i;
                container.XY.second = j;
                
                tempCell.container = container;

                // add to hash map
                string key = weight+description;
                initState.hashMapForContainer[key].push_back(container);
                container.key = key;

                // update containerInColumn
                initState.numOfcontainerInColumn[j].second++;
                
                // lowest container in column
                if(initState.numOfcontainerInColumn[j].first <= i){
                    initState.numOfcontainerInColumn[j].first = i+1;
                }

                initState.ship[i][j] = tempCell;
            }
        }
    }

    for(int i = 0; i < 12; i++){
        calculateNumContainerAbove(i,initState);
    }
}

// For Unit test
Container mainApp::getContainer(int x, int y){
    if(initState.ship[x][y].status == USED){
        return initState.ship[x][y].container;
    }
    return noContainer;
}

// NEED TO TEST
void mainApp::calculateNumContainerAbove(int column, State &currState){
    
    int currentNumOfContainer = currState.numOfcontainerInColumn[column].second;
    
    int currentContainer = currState.numOfcontainerInColumn[column].first-1;
    
    if(currentNumOfContainer == 0){
        return;
    }
    
    int numAbove = 0;
    while(currentNumOfContainer > 0){
        currState.ship[currentContainer][column].container.numContainerAbove = numAbove;
        numAbove++;
        currentNumOfContainer--;
        currentContainer--;
    }
}

// NEED TO TEST
void mainApp::updateNumContainerAbove(int column, int numChanged, State &currState){

    currState.numOfcontainerInColumn[column].second += numChanged;
    currState.numOfcontainerInColumn[column].first += numChanged;

    int currContainer = currState.numOfcontainerInColumn[column].first-1;
    int numOfContainer = currState.numOfcontainerInColumn[column].second;

    int numAbove = 0;
    while(numOfContainer > 0){
        currState.ship[currContainer][column].container.numContainerAbove = numAbove;
        numAbove++;
        numOfContainer--;
        currContainer--;
    }

}

// NEED TO TEST
// return container with key that has the least container above it, removes from hashmap
Container mainApp::getContainerWithKey(string key, State &currState){
    vector<Container> temp = currState.hashMapForContainer[key];
    sort(temp.begin(), temp.end(), greater_than_key());
    Container unload = temp[temp.size()-1];

    temp.pop_back();
    return unload;
}

// NEED TO TEST
pair<int,int> mainApp::findHighestColumnBetween(pair<int,int> orig, pair<int,int> dest, State &currState){
    int left = dest.second;
    int right = orig.second;
    if(left > right){
        left = orig.first;
        right = dest.first;
    }

    int highestColumn = left;

    // find the highest column between crane and container
    for(int i = left+1; i < right; i++){
        if(left == -1){
            i++;
        }
        if(currState.numOfcontainerInColumn[highestColumn].first-1 < currState.numOfcontainerInColumn[i].first-1){
            highestColumn = i;
        }
    }
    pair<int,int> highestColumnBetween;
    highestColumnBetween.second = highestColumn;
    highestColumnBetween.first = currState.numOfcontainerInColumn[highestColumn].first;
    
    return highestColumnBetween;
}

// NEED TO TEST
// status 1 = moving to truck
// status -1 = move to buffer
// status 0 = move within ship
void mainApp::moveContainer(int destColumn, Container container, State currState, State &newState ,int status){
    
    int highestColumn = -1;
    pair<int,int> dest;
    dest.first = currState.numOfcontainerInColumn[destColumn].first;
    dest.second = destColumn;
    
    pair<int,int> orig = container.XY;

    int timeToMoveCrane = 0;

    // check if crane is currently at orig position
    if(currState.craneLocation.first != orig.first || currState.craneLocation.second != orig.second){
        // not at orig position, move crane to orig position
        pair<int,int> cranePos = currState.craneLocation;
        
        pair<int,int> highestColumnBetween = findHighestColumnBetween(cranePos,orig,currState);

        timeToMoveCrane += calculateTime(cranePos,highestColumnBetween);
        timeToMoveCrane += calculateTime(highestColumnBetween,orig);

        cranePos = orig;
        currState.craneLocation = cranePos;
    }
    
    int timeToMove = 0;
    if(status == 1){
        timeToMove += calculateTime(orig,pinkCell) + TIME_FROM_SHIP_TO_TRUCK;
        currState.craneLocation = pinkCell;
        currState.craneState = TRUCK;
        currState.ship[container.XY.first][container.XY.second].status = UNUSED;

        updateNumContainerAbove(orig.second,-1,currState);
        
    }
    else if(status == -1){
        timeToMove += calculateTime(orig,pinkCell) + TIME_FROM_SHIP_TO_BUFFER;
        currState.craneLocation = pinkCell;
        currState.craneState = BUFFER;
        currState.ship[container.XY.first][container.XY.second].status = UNUSED;
        moveToBuffer(currState,container);

        updateNumContainerAbove(orig.second,-1,currState);
    }
    else if(status == 0){
        // check if column we are moving to has enough space
        if(currState.numOfcontainerInColumn[destColumn].first < ROW_SHIP){
            
            pair<int,int> highestColumnBetween = findHighestColumnBetween(dest,orig,currState);
            timeToMove += calculateTime(orig,highestColumnBetween);
            timeToMove += calculateTime(highestColumnBetween,dest);

            currState.craneState = SHIP;
            
            currState.craneLocation = dest;

            currState.ship[container.XY.first][container.XY.second].status = UNUSED;
            currState.ship[dest.first][dest.second].status = USED;
            currState.ship[dest.first][dest.second].container = container;
            container.XY = dest;

            // update state
            updateNumContainerAbove(orig.second, -1, currState);
            updateNumContainerAbove(dest.second, 1, currState);
            }
    }

    currState.time = timeToMove + timeToMoveCrane;
    currState.cost = currState.time * (currState.toBeLoaded.size() + currState.toBeUnloaded.size());
    newState = currState;
}

void mainApp::moveToBuffer(State &currState, Container container){
    for(int i = 0; i < COLUMN_BUFFER;i++){
        for(int j = 0; j < ROW_BUFFER; j++){
            if(currState.buffer[j][i].status == UNUSED){
                currState.buffer[j][i].container = container;
            }
        }
    }
}


State mainApp::unload_load(vector<string> &toBeUnloaded, vector<Container> &toBeLoaded){

    // convert toBeUnloaded in to vector of containers
    for(int i = 0; i < toBeUnloaded.size(); i++){
        initState.toBeUnloaded.push_back(getContainerWithKey(toBeUnloaded[i],initState));
    }
    
    // sort container from biggest to smallest
    sort(initState.toBeUnloaded.begin(), initState.toBeUnloaded.end(), greater_than_key());
    initState.toBeLoaded  = toBeLoaded;

    bestState.emplace(initState);

    State currState;
    
    // unload only
    if(currState.toBeUnloaded.size() > 0 && currState.toBeLoaded.size() == 0 ){
        
        while(bestState.size() > 0){
            currState = bestState.top();
            bestState.pop();
            
            if(currState.toBeUnloaded.size() == 0){
                // found best solution
                return currState;
            }
            unload_one(currState);
        }
    }

    // load only
    if(currState.toBeLoaded.size() > 0 && currState.toBeUnloaded.size() == 0 ){
        
        while(bestState.size() > 0){
            currState = bestState.top();
            bestState.pop();
            
            if(currState.toBeLoaded.size() == 0){
                // found best solution
                return currState;
            }
            load_one(currState);
        }
    }

    // from ship to buffer
    // from buffer to ship
    
    
}

void mainApp::unload_one(State currState){
    sort(currState.toBeUnloaded.begin(),currState.toBeUnloaded.end(),greater_than_key());

    Container currContainer = currState.toBeUnloaded.at(currState.toBeUnloaded.size()-1);

    if(currContainer.numContainerAbove > 0){
        // move container to each column
        
        int row = currState.numOfcontainerInColumn[currContainer.XY.second].first-1;
        int column = currContainer.XY.second;
        Container topContainer = currState.ship[row][column].container;

        for(int i = 0; i < COLUMN_SHIP; i++){

            State newState;
            // not in the same column
            if(i != currContainer.XY.second){
                moveContainer(i,topContainer,currState,newState,0);
                // check if a move was made
                if(newState.cost != currState.cost){
                    bestState.emplace(newState);
                }
            }
        }
        // move to buffer -- TODO

    }
    else{
        State newState;
        moveContainer(0,currContainer,currState,newState,1);
        bestState.emplace(newState);
    }
}

void mainApp::load_one(State currState){
    
}