#include "../header/mainApp.h"
#include <fstream>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

using namespace std;

struct greater_than_key
{
    inline bool operator() (const Container &struct1, const Container &struct2)
    {
        return (struct1.numContainerAbove > struct2.numContainerAbove);
    }
};

// constructor
mainApp :: mainApp() {
    pinkCell.first = ROW_SHIP;
    pinkCell.second = -1;

    pinkCellBuffer.first = ROW_BUFFER;
    pinkCellBuffer.second = COLUMN_BUFFER;
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
    initState.cost = 0;
    initState.time = 0;

    int index = 0;
    for(unsigned int i = 0; i < manifest.size(); i++){
        if(manifest[i] == '/' || manifest[i] == '\\'){
            index = i;
        }
    }
    string shipName = manifest.substr(index+1,manifest.size()-5 - index);
    
    this->currShipName = shipName;
}

void mainApp::parseManifest(){
    manifest.open(manifestName);
    if(!manifest.is_open()){
        cout << "File not open" << endl;
        return;
    }
    string currLine;
    int counter = 0;
    
    // parse manifest
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 12; j++){
            getline(manifest, currLine);
        
            string weight = currLine.substr(10,5);

            string description = currLine.substr(18,currLine.size()-1);
            
            if(description == "NAN"){
                Cell tempCell;
                tempCell.status = NOTEXIST;
                tempCell.XY.first = i;
                tempCell.XY.second = j;

                initState.ship[i][j] = tempCell;
            }

            else if(description == "UNUSED"){
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
                
                // add to hash map
                string key = weight+description;
                container.key = key;
                tempCell.container = container;

                initState.hashMapForContainer[key].push_back(container);

                // update containerInColumn
                initState.numOfcontainerInColumn[j].second++;
                
                // lowest container in column
                if(initState.numOfcontainerInColumn[j].first <= i){
                    initState.numOfcontainerInColumn[j].first = i+1;
                }

                initState.ship[i][j] = tempCell;
                counter++;
            }
        }
    }

    for(int i = 0; i < COLUMN_SHIP; i++){
        calculateNumContainerAbove(i,initState);
    }

    addComments("Manifest located at " + manifestName + " is opened, there are " + to_string(counter) + " containers on the ship");

}

int mainApp::calculateTime(pair<int,int> &a, pair<int,int> &b){
    return abs(a.first - b.first) + abs(a.second - b.second);
}

// For Unit test
Container mainApp::getContainer(int x, int y){
    if(initState.ship[x][y].status == USED){
        return initState.ship[x][y].container;
    }
    return noContainer;
}

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

    // update container in toBeUnloaded
    for(int i = 0; i < currState.toBeUnloaded.size(); i++){
        int row = currState.toBeUnloaded[i].XY.first;
        int column2 = currState.toBeUnloaded[i].XY.second;
        
        currState.toBeUnloaded[i].numContainerAbove = currState.ship[row][column2].container.numContainerAbove;
    }

    for(int i = 0; i <  currState.numOfcontainerInColumn[column].first; i++){
        string key = currState.ship[i][column].container.key;
        int size_of_vec_container = currState.hashMapForContainer[key].size();
        
        for(int j = 0; j < size_of_vec_container; j++){
            if(currState.hashMapForContainer[key][j].XY == currState.ship[i][column].container.XY){
                int row = currState.ship[i][column].container.XY.first;
                int column2 = currState.ship[i][column].container.XY.second;
                
                currState.hashMapForContainer[key][j].numContainerAbove = currState.ship[row][column2].container.numContainerAbove;
            }
        }
    }

}


// return container with key that has the least container above it
Container mainApp::getContainerWithKey(string &key, State &currState){
    vector<Container> temp = currState.hashMapForContainer[key];
    
    sort(temp.begin(), temp.end(), greater_than_key());
    Container unload = temp[temp.size()-1];
    
    // update numOfContainerAbove
    pair<int,int> a = unload.XY;
    int numAbove = currState.ship[a.first][a.second].container.numContainerAbove;
    unload.numContainerAbove = numAbove;


    return unload;
}

pair<int,int> mainApp::findHighestColumnBetween(pair<int,int> &orig, pair<int,int> &dest, State &currState){
    int left = dest.second;
    int right = orig.second;
    pair<int,int> highestColumnBetween;
    if(left > right){
        left = orig.second;
        right = dest.second;
    }

    if(abs(orig.second - dest.second) <=1 ){
        highestColumnBetween.first = max(orig.first,dest.first);
        highestColumnBetween.second = max(orig.second, dest.second);
        return highestColumnBetween;
    }

    int highestColumn = left+1;

    for(int i = left+1; i <= right; i++){
        if(left == -1){
            i++;
        }
        if(currState.numOfcontainerInColumn[highestColumn].first < currState.numOfcontainerInColumn[i].first){
            highestColumn = i;
        }
    }
    
    highestColumnBetween.second = highestColumn;
    highestColumnBetween.first = currState.numOfcontainerInColumn[highestColumn].first;
    
    return highestColumnBetween;
}

// status 1 = moving to truck
// status -1 = move to buffer
// status 0 = move within ship
// status 2 = truck to ship
// status 3 = buffer to ship
bool mainApp::moveContainer(int destColumn, Container &container, State &currState ,int status){
    
    pair<int,int> dest;
    dest.first = currState.numOfcontainerInColumn[destColumn].first;
    dest.second = destColumn;
    
    pair<int,int> orig = container.XY;

    int timeToMoveCrane = 0;

    bool moved = true;
    
    pair<int,int> cranePos = currState.craneLocation;

    // ship to truck
    int timeToMove = 0;
    if(status == 1){

        if(currState.craneState == TRUCK){
            currState.craneState = SHIP;
            currState.time+= TIME_FROM_SHIP_TO_TRUCK;
            currState.craneLocation = pinkCell;
        }
        if(currState.craneState == BUFFER){
            timeToMove += calculateTime(cranePos,pinkCellBuffer);
            currState.craneState = SHIP;
            currState.time += TIME_FROM_SHIP_TO_BUFFER;
            currState.craneLocation = pinkCell;

        }
        if(currState.craneState == SHIP){
            currState.craneState = TRUCK;
            currState.time+= TIME_FROM_SHIP_TO_TRUCK;
            timeToMoveCrane += calculateTime(cranePos,orig);

            
        }
        
        timeToMove += calculateTime(orig,pinkCell);
        currState.craneLocation = pinkCell;
        currState.ship[container.XY.first][container.XY.second].status = UNUSED;

        addMoveOrder(currState,orig,pinkCell,pinkCell);


        updateNumContainerAbove(orig.second,-1,currState);
        
    }
    // ship to buffer
    else if(status == -1){
        timeToMoveCrane += calculateTime(orig, cranePos);
        timeToMove += calculateTime(orig,pinkCell) + TIME_FROM_SHIP_TO_BUFFER;
        currState.craneLocation = pinkCellBuffer;
        currState.craneState = BUFFER;
        currState.ship[container.XY.first][container.XY.second].status = UNUSED;
        
        addMoveOrder(currState,orig,pinkCell,pinkCell);
        moveToBuffer(currState,container);

        updateNumContainerAbove(orig.second,-1,currState);
    }
    // within ship
    else if(status == 0){
        // check if column we are moving to has enough space
        if(currState.numOfcontainerInColumn[destColumn].first < ROW_SHIP){
            
            pair<int,int> highestColumnBetween = findHighestColumnBetween(dest,orig,currState);
            timeToMove += calculateTime(orig,dest);

            if(currState.craneState == TRUCK){
                currState.craneState = SHIP;
                currState.time+=TIME_FROM_SHIP_TO_TRUCK;
            }
            if(currState.craneState == SHIP){
                timeToMoveCrane = calculateTime(cranePos,orig);
            }

            currState.craneState = SHIP;
            
            currState.craneLocation = dest;

            currState.ship[container.XY.first][container.XY.second].status = UNUSED;
            currState.ship[dest.first][dest.second].status = USED;
            currState.ship[dest.first][dest.second].container = container;
            container.XY = dest;

            // update state
            updateNumContainerAbove(orig.second, -1, currState);
            updateNumContainerAbove(dest.second, 1, currState);

            timeToMove += 2;

            addMoveOrder(currState,orig,highestColumnBetween,dest);
        }
        else{
            moved = false;
        }
    }
    // truck to ship
    else if(status == 2){
        
        // check if column we are moving to has enough space
        if(currState.numOfcontainerInColumn[destColumn].first < ROW_SHIP){

            if(currState.craneState == SHIP){
                currState.craneState = TRUCK;
                currState.time+=TIME_FROM_SHIP_TO_TRUCK;
                timeToMoveCrane += calculateTime(cranePos,pinkCell);
            }
            if(currState.craneState == BUFFER){
                currState.craneState = SHIP;
                currState.time+=TIME_FROM_SHIP_TO_BUFFER;
                timeToMoveCrane += calculateTime(cranePos,pinkCellBuffer);
                currState.craneLocation = pinkCell;

            }
            if(currState.craneState == TRUCK){
                currState.craneState = SHIP;
                currState.time+=TIME_FROM_SHIP_TO_TRUCK;
            }
            
            currState.craneLocation = dest;

            timeToMove += calculateTime(pinkCell, dest);
            currState.ship[dest.first][dest.second].status = USED;
            currState.ship[dest.first][dest.second].container = container;

            
            container.XY = dest;
            
            addMoveOrder(currState,pinkCell,pinkCell,dest);


            // update state
            updateNumContainerAbove(dest.second, 1, currState);
        }
        else{
                moved = false;
        }
    }
    // buffer to ship
    else if(status == 3){

        if(currState.craneState == SHIP){
            currState.craneState = BUFFER;
            currState.time+=TIME_FROM_SHIP_TO_BUFFER;
            timeToMove+=calculateTime(cranePos,pinkCell);
        }
        if(currState.craneState == TRUCK){
            currState.craneState = BUFFER;
           timeToMoveCrane+=TIME_FROM_SHIP_TO_BUFFER;
        }
        timeToMove += calculateTime(pinkCellBuffer, container.XY);
        addMoveOrder(currState,currState.buffer.top().XY,pinkCellBuffer,pinkCellBuffer);

        int emptyColumn = calculateEmptyColumn(currState, -1);
        moveContainer(emptyColumn,container,currState,2);
    }
    currState.time += timeToMove + timeToMoveCrane;
    currState.cost = currState.time + currState.toBeLoaded.size() + currState.toBeUnloaded.size() + currState.buffer.size();



    return moved;
}

void mainApp::moveToBuffer(State &currState, Container container){

    if(currState.buffer.empty()){
        container.XY.first == 0;
        container.XY.second == COLUMN_BUFFER-1;
        currState.buffer.push(container);
    }
    else{
        Container topContainer = currState.buffer.top();
        
        if(topContainer.XY.first+1 >= ROW_BUFFER){
            container.XY.first == 0;
            container.XY.second = topContainer.XY.second-1;
        }
        else{
            container.XY.first = topContainer.XY.first+1;
            container.XY.second = topContainer.XY.second;
        }
        currState.buffer.push(container);
    }
    currState.craneLocation = container.XY;
    currState.time += calculateTime(pinkCellBuffer, container.XY);
    addMoveOrder(currState,pinkCellBuffer,pinkCellBuffer,currState.buffer.top().XY);
}

State mainApp::unload_load(vector<string> &toBeUnloaded, vector<Container> &toBeLoaded){

    if(toBeLoaded.empty() && toBeUnloaded.empty()) return initState;

    // convert toBeUnloaded in to vector of containers
    for(int i = 0; i < toBeUnloaded.size(); i++){
        initState.toBeUnloaded.push_back(getContainerWithKey(toBeUnloaded[i],initState));
    }
    // sort container from biggest to smallest
    sort(initState.toBeUnloaded.begin(), initState.toBeUnloaded.end(), greater_than_key());
    initState.toBeLoaded  = toBeLoaded;
   
    bestState.emplace(initState);

    State currState;
    
    while(bestState.size() > 0){
        
        currState = bestState.top();
        bestState.pop();


        if(currState.toBeLoaded.size() == 0 && currState.toBeUnloaded.size() == 0 && currState.buffer.empty()){
            // found best solution
            solutionState = currState;
            return currState;
        }

        // unload only
        else if(currState.toBeUnloaded.size() > 0 && currState.toBeLoaded.size() == 0 ){
            unload_one(currState);
        }

        // load only
        else if(currState.toBeLoaded.size() > 0 && currState.toBeUnloaded.size() == 0 ){
  
            load_one(currState);
        }

        // load and unload
        else if(currState.toBeLoaded.size() > 0 && currState.toBeUnloaded.size() > 0 ){
            // crane on ship then unload
            if(currState.craneState == SHIP){
                unload_one(currState);
                load_one(currState);
            }
            // crane on truck then load
            else if(currState.craneState == TRUCK){
                load_one(currState);
                unload_one(currState);
            }
            
        }

        // buffer to ship
        else if(!currState.buffer.empty()){
            Container currContainer = currState.buffer.top();
            currState.buffer.pop();
            moveContainer(-1,currContainer,currState,3);
            bestState.emplace(currState);
        }

    }
    
    
    return initState;
}

void mainApp::unload_one(State &currState){
    sort(currState.toBeUnloaded.begin(),currState.toBeUnloaded.end(),greater_than_key());

    Container currContainer = currState.toBeUnloaded.at(currState.toBeUnloaded.size()-1);

    if(currContainer.numContainerAbove > 0){
        
        int row = currState.numOfcontainerInColumn[currContainer.XY.second].first-1;
        int column = currContainer.XY.second;
        Container topContainer = currState.ship[row][column].container;

        for(int i = 0; i < COLUMN_SHIP; i++){

            State newState;
            // not in the same column
            if(i != currContainer.XY.second){
                newState = currState;
                int emptyColumn = calculateEmptyColumn(currState,currContainer.XY.second);

                if(emptyColumn == -2){
                    moveContainer(emptyColumn,topContainer,newState,-1);
                    bestState.emplace(newState);
                    break;
                }
                // found a column that we can put on
                else if(emptyColumn != -1){
                    moveContainer(emptyColumn,topContainer,newState,0);
                    bestState.emplace(newState);
                    break;
                }
                else{
                    moveContainer(i,topContainer,newState,0);
                    bestState.emplace(newState);
                }
            }
        }

    }
    else{
        State newState;
        newState = currState;
        moveContainer(0,currContainer,newState,1);
        newState.toBeUnloaded.pop_back();


        bestState.emplace(newState);

    }
}

void mainApp::load_one(State &currState){
    Container currContainer = currState.toBeLoaded.at(currState.toBeLoaded.size()-1);

    for(int i = 0; i < COLUMN_SHIP; i++){
        State newState;
        newState = currState;
        
        int emptyColumn = calculateEmptyColumn(currState, -1);
        
        // nothing to load, place where ever is free
        if(currState.toBeUnloaded.size() == 0){
            if(moveContainer(emptyColumn,currContainer,newState,2)){
                newState.toBeLoaded.pop_back();
                bestState.emplace(newState);
                break;
            }
        }

        // have something to load, but there are free column to use
        else if(emptyColumn != -1){
            moveContainer(emptyColumn,currContainer,newState,2);
            newState.toBeLoaded.pop_back();
            bestState.emplace(newState);
            break;
        } 
        
        // no free column to use, try to place in every column
        else if(moveContainer(i,currContainer,newState,2)){
            newState.toBeLoaded.pop_back();
            bestState.emplace(newState);
        }
    }

}

// return -1 if all column have container we need to unload
// return -2 if all column are full then need to move to buffer
int mainApp::calculateEmptyColumn(State& currState, int column){
    bool arr[12] = {false};
    int index = -1;
    bool isFull = true;
    for(int i = 0; i < currState.toBeUnloaded.size(); i++){
        arr[currState.toBeUnloaded[i].XY.second] = true;
    }

    pair<int,int> orig;
    orig.second = column;
    orig.first = currState.numOfcontainerInColumn[column].first;
    
    for(int i = 0; i < COLUMN_SHIP; i++){
        if(arr[i] == false && currState.numOfcontainerInColumn[i].first < ROW_SHIP && i != column){
            if(column == -1){
                return i;
            }
            else if(index == -1){
                index = i;
            }
            // else if(abs(i - column) < abs(index - column)){
            //     index = i;
            // }
            else{
                pair<int,int> best;
                best.first = currState.numOfcontainerInColumn[index].first;
                best.second = index;

                pair<int,int> c;
                c.first = currState.numOfcontainerInColumn[i].first;
                c.second = i;
                if(calculateTime(best,orig) > calculateTime(orig,c)){
                    index = i;
                }
            }
        }
    }
    
    for(int i = 0; i < COLUMN_SHIP; i++){
        if(currState.ship[ROW_SHIP-1][i].status == UNUSED && i != column){
            isFull = false;
        }
    }

    if(isFull){
        return -2;
    }

    return index;
}

void mainApp::addMoveOrder(State &currState, pair<int,int> orig, pair<int,int> midPoint, pair<int,int> dest){
    vector<pair<int,int>> moveOrder;

    moveOrder.push_back(orig);

    if(orig != pinkCell && dest != pinkCell){
        while(orig != dest){
                // dest is on the left
                if(orig.second > dest.second){
                    orig.second--;
                }
                // dest is on the right
                else if(orig.second < dest.second){
                    orig.second++;
                }
                // same column as dest but dest is below
                else if(orig.first > dest.first){
                    orig.first--;
                }
                else{
                    orig.first++;
                }

                moveOrder.push_back(orig);
        }
        currState.containerMoveOrder.push_back(moveOrder);
        return;
    }

    while(orig != midPoint){
        // check if mid point is above orig, if above then we have to go up and over
        if(midPoint.first > orig.first){
            orig.first++;
        }
        // midpoint is on the left
        else if(orig.second > midPoint.second){
            orig.second--;
        }
        // midpoing is on the right
        else if(orig.second < midPoint.second){
            orig.second++;
        }
        else if(midPoint.first < orig.first){
            orig.first--;
        }
        moveOrder.push_back(orig);
    }

    while(orig != dest){


        // dest is on the left
        if(orig.second > dest.second){
            orig.second--;
        }
        // dest is on the right
        else if(orig.second < dest.second){
            orig.second++;
        }
        // same column as dest but dest is below
        else if(orig.first > dest.first){
            orig.first--;
        }
        else{
            orig.first++;
        }

        moveOrder.push_back(orig);
    }

    currState.containerMoveOrder.push_back(moveOrder);

}

vector<pair<int,int>> mainApp::getNextMoveSequence(){
    vector<pair<int,int>> List;
    if(currMoveSequence < solutionState.containerMoveOrder.size()){
       List = solutionState.containerMoveOrder[currMoveSequence];
        currMoveSequence++;
        pair<int,int> orig = List[0];
        pair<int,int> dest = List[List.size()-1];

        if(orig == pinkCell){
           for(int i = 0; i < initState.toBeLoaded.size(); i++){
             if(solutionState.ship[dest.first][dest.second].container.description == initState.toBeLoaded[i].description){
                // get container weight 
             }
           } 
           string containerDescription = solutionState.ship[dest.first][dest.second].container.description;
           addComments('"' + containerDescription + '"' + " is loaded");
        }
        if(dest == pinkCell){
            string containerDescription = initState.ship[orig.first][orig.second].container.description;
            addComments('"' + containerDescription + '"' + " is offloaded");
        }
    }
    else{
        addComments("Finished a cycle.");
        createManifest();
    }
    return List;
}

int mainApp::getEstimatedTimeInMin(){
    return solutionState.time;
}

void mainApp::addComments(string comment){
    auto time = std::chrono::system_clock::now();
    std::time_t end_time = std::chrono::system_clock::to_time_t(time);

    std::ofstream Log_file ("Log.txt", std::ios::app);

    Log_file << std::ctime(&end_time) << ":       " <<comment << endl;

    Log_file.close();

}

void mainApp::createManifest(){

    TCHAR path[MAX_PATH] = {0};

    SHGetSpecialFolderPath(NULL, path, CSIDL_DESKTOPDIRECTORY, FALSE);

    std::string pathtmp(&path[0], &path[255]);
    int counter = 0;
    for(int i = 0; i< pathtmp.size(); i++){
        if(isalpha(pathtmp[i])){
            counter = i;
        }
    }
    pathtmp.resize(counter+1);

    string manifestName = pathtmp + "/" + currShipName + "OUTBOUND.txt";

    for(int i = 0; i < manifestName.size(); i++){
        if(manifestName[i] == '\\'){
            manifestName[i] = '/';
        }
    }
    
    std::ofstream manifest (manifestName);

    for(int i = 0; i < ROW_SHIP; i++){
        for(int j = 0; j < COLUMN_SHIP; j++){
            manifest << "[" << std::setfill('0') << std::setw(2) << i+1 << "," << std::setfill('0') << std::setw(2) << j+1 << "], ";


            if(solutionState.ship[i][j].status == NOTEXIST){
                manifest << "{00000}, ";
                manifest << "NAN" << endl;
            }
            else if (solutionState.ship[i][j].status == UNUSED){
                manifest << "{00000}, ";
                manifest << "UNUSED" << endl;
            }
            else{
                manifest << "{" << std::setfill('0') << std::setw(5) << solutionState.ship[i][j].container.weight << "}, ";
                manifest << solutionState.ship[i][j].container.description << endl;
            }

        }
    }

    addComments("Manifest " + manifestName + " was written to desktop, and a reminder pop-up is displayed");
}

int mainApp::numOfMovesRemain(){
    return solutionState.containerMoveOrder.size()-currMoveSequence;
}