#pragma warning(disable : 4996)

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
        if(struct1.numContainerAbove == struct2.numContainerAbove){
            return (abs(struct1.XY.first - 8) + abs(struct1.XY.second +1)) > (abs(struct2.XY.first - 8) + abs(struct2.XY.second +1));
            //return struct1.XY.second > struct2.XY.second;
        }
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

            //string description = currLine.substr(18,currLine.size()-1);
            description.erase(description.find_last_not_of(" \n\r\t")+1); //get rid of white space at end of line
            if(description == "NAN") {
                //cout << "At " << i << ", " << j << " : NAN" << endl;
                Cell tempCell;
                tempCell.status = NOTEXIST;
                tempCell.XY.first = i;
                tempCell.XY.second = j;

                initState.ship[i][j] = tempCell;
            }

            else if(description == "UNUSED"){
                //cout << "At " << i << ", " << j << " : UNUSED" << endl;
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
                //cout << "At " << i << ", " << j << " : Container here" << endl;
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
        updateNumContainerAbove(i,0,initState);
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
        addMoveOrder(currState,container.XY,pinkCellBuffer,pinkCellBuffer);

        int emptyColumn = calculateEmptyColumn(currState, -1);
        moveContainer(emptyColumn,container,currState,2);
    }
    currState.time += timeToMove + timeToMoveCrane;
    currState.cost = currState.time + (currState.toBeLoaded.size() + currState.toBeUnloaded.size() + currState.buffer.size())*10;



    return moved;
}

void mainApp::moveToBuffer(State &currState, Container container){

    if(currState.buffer.empty()){
        container.XY.first = 0;
        container.XY.second = COLUMN_BUFFER-1;
        currState.buffer.push(container);
    }
    else{
        Container topContainer = currState.buffer.top();
        
        if(topContainer.XY.first+1 >= ROW_BUFFER){
            container.XY.first = 0;
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
    if(!currState.hashMapForContainer[currContainer.key].empty()){
        currContainer = getContainerWithKey(currState.toBeUnloaded.at(currState.toBeUnloaded.size()-1).key, currState);
    }
    //Container currContainer = getContainerWithKey(currState.toBeUnloaded.at(currState.toBeUnloaded.size()-1).key, currState);
    
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
        removeContainer(currContainer,newState);
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




//-------------balance changes--------------


float mainApp::calculateLeftSideWeight(State &currState){
        float weightLeftSide = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 6; j++) {
                if (j < 6) { 
                    if (currState.ship[i][j].status == USED) {
                        weightLeftSide += currState.ship[i][j].container.weight;
                    }
                }
            }
        }
        return weightLeftSide;
    }
float mainApp::calculateRightSideWeight(State &currState){
        float weightRightSide = 0;
        for (int i = 0; i < 8; i++) {
            for (int j = 6; j < 12; j++) {
                if (currState.ship[i][j].status == USED) {
                    weightRightSide += currState.ship[i][j].container.weight;
                }
            }
        }
        return weightRightSide;
    }

float mainApp::calculateBalance(State &currState){
        //cout << "calculating balance" << endl;
        
        return calculateRightSideWeight(currState)/calculateLeftSideWeight(currState);
}

string mainApp::generateBalanceString(State currState) { //pseudohash
    string shipKey = "|";
    for (int i = 7; i > -1; i--) {
        for (int j = 0; j < 12; j++) {
            if (currState.ship[i][j].status == UNUSED) {
                shipKey += "U|";
            }
            else if (currState.ship[i][j].status == NOTEXIST) {
                shipKey += "-|";
            }
            else {
                shipKey += currState.ship[i][j].container.weight + " | ";
            }
        }
    }
    
    return shipKey;   
    
}

State mainApp::balanceSearch(){ //balance
	if (siftCheck()) {
        //cout << "SIFT is necessary" << endl;
        return siftProcedure();
    }
    
    bestStateBalance.emplace(initState);
	
    State currState;

    stack<Container> toBeBalanced;
    Container currCont;
        //int leastRemainingTime = 999999;
    int iter = 0;
    int duplicates = 0;
    vector<string> checkDuplicates;
    string shipKey = "";
    bool doNotExpand = false;
    while(bestStateBalance.size() > 0){
        doNotExpand = false;
            //cout << "iter : " << iter << endl;
            //cout << "priority queue size : " << bestStateBalance.size() << endl;
            
        iter++;
            
        currState = bestStateBalance.top();
        bestStateBalance.pop();

           
        shipKey = generateBalanceString(currState);
        for (int i = 0; i < checkDuplicates.size(); i++) {
            if (shipKey == checkDuplicates.at(i)) {
                doNotExpand = true;
            }
        }
        if (doNotExpand == true) {
            //cout << "duplicates found" << endl;
            duplicates++;
            continue;
        }
        else {
            checkDuplicates.push_back(shipKey);
            
                //cout << "checking" << endl;
            float curBal = calculateBalance(currState);
            currState.depth++;
                //currState.balance = curBal;
            if( (curBal > 0.89) && (curBal < 1.11) && (currState.buffer.empty())){
                    // found best solution
                    //std::cout << "goal reached" << std::endl;
                //std::cout << calculateBalance(currState) << std::endl;
                    //std::cout << "goal cost: " << currState.cost << std::endl;
                cout << "Iterations : " << iter << endl;
                cout << "Duplicates : " << duplicates << endl;
                solutionState=currState;
                return solutionState;
            }

                // balance
            else if((curBal < 0.9) || (curBal > 1.1)){ 
                    //cout << "checking 2" << endl;
                toBeBalanced = balanceList(currState);
                    //cout << "depth: " << currState.depth << endl;
                    //cout << curBal << endl;
                while (!toBeBalanced.empty()) {
                        //cout << "checking" << toBeBalanced.size() << endl;
                    currCont = toBeBalanced.top();
                    toBeBalanced.pop();
                    balance_one(currState, currCont);
                }
                    
            }

                // buffer to ship
            else if(!currState.buffer.empty()){
                    //cout << "buffer enter" << endl;
                Container currContainer = currState.buffer.top();
                currState.buffer.pop();
                moveContainerBalance(-1,currContainer,currState,3);
        
                bestStateBalance.emplace(currState);
            }

        }
            
            
        
    }
    return initState;
}

bool mainApp::siftCheck() { 
    State dummyState = initState;
    Container currContainer;
    vector<Container> weightList1;
    vector<Container> weightList2; 
    float weight1 = 0;
    float weight2 = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
                //cout << "at " << i << ", " << j << endl;
            if (dummyState.ship[i][j].status == USED) {
                weightList1.push_back(dummyState.ship[i][j].container);     
            }
        }
    }
    float differenceWeight = 99999;
    float dif = differenceWeight;
    int noChange = 0;
    int moveNumber = -1;
    while(1) {
        if (noChange > 500) {
            break;
        }
        for (int i = 0; i < weightList1.size(); i++) {
            weight1 += weightList1.at(i).weight;
        }
        for (int i = 0; i < weightList2.size(); i++) {
            weight2 += weightList2.at(i).weight;
        }
        //cout << weight2/weight1 << endl;
        if ((weight2/weight1) > 0.89 && (weight2/weight1) < 1.11) {
            return false;
        }
        else if ((weight2/weight1) < 0.9) { //less weight on right side then left
            //std::cout << "less weight on right than left" << std::endl;
            for (int i = 0; i < weightList1.size(); i++) {
                dif = abs((weight2 + weightList1.at(i).weight) - (weight1 - weightList1.at(i).weight));
                    //cout << "dif " << dif << endl; 
                if ( dif < differenceWeight ) {
                    differenceWeight = dif;
                            //dummyState.balanceDifference = dif;
                    moveNumber = i;
                    noChange = 0;
                }
                else {
                    noChange++;
                }
            }
            if (moveNumber > -1) {
                currContainer = weightList1.at(moveNumber);
                //cout << "currContainer weight: " << currContainer.weight << endl;
                weightList2.push_back(currContainer);
                weightList1.erase(weightList1.begin()+moveNumber);
                
            }
                    //cout << "currContainer selected at : " << i << ", " << j << endl;
        }
        
        else if ((weight2/weight1) > 1.1) { // less weight on left than right
            //std::cout <<"less weight on left than right" << std::endl;
            for (int i = 0; i < weightList2.size(); i++) {
                
                dif = abs((weight1 + weightList2.at(i).weight) - (weight2 - weightList2.at(i).weight));
                if ( dif < differenceWeight ) {
                    differenceWeight = dif;
                            //dummyState.balanceDifference = dif;
                    moveNumber = i;
                    //cout << "currContainer selected at : " << i << ", " << j << endl;
                    noChange = 0;
                }
                else {
                    noChange++;
                }
                    
                
            }
            if (moveNumber > -1) {
                currContainer = weightList2.at(moveNumber);
                        
                weightList1.push_back(currContainer);
                weightList2.erase(weightList2.begin()+moveNumber);
            }
        }
        moveNumber = -1;
        weight1 = 0;
        weight2 = 0;
    }
    return true;
    
    
}
pair<int,int> mainApp::findNearestContainer(int weight, pair<int,int> dest, State currState) { //For sift, find nearest container to dest with same weight
    if (currState.ship[dest.first][dest.second].container.weight == weight && currState.ship[dest.first][dest.second].status == USED ) {
        return dest;
    }
    int timeCost = 99999;
    pair<int, int> orig;
    pair<int, int> finalOrig;
    finalOrig.first = -1;
    finalOrig.second = -1;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            if ((currState.ship[i][j].status == USED) && (currState.ship[i][j].container.siftPlaced == false) && (currState.ship[i][j].container.weight == weight)) {
                orig.first = i;
                orig.second = j;
                int cost = calculateTime(orig, dest);
                if (cost < timeCost) {
                    timeCost = cost;
                    finalOrig = orig;
                }
            }
        }
    }
    if (finalOrig.first == -1 && finalOrig.second == -1) {
        //cout << "something went wrong" << endl;
    }
    int aboveCont = currState.ship[finalOrig.first][finalOrig.second].container.numContainerAbove;
    if ((aboveCont > 0) && (currState.ship[aboveCont+finalOrig.first][finalOrig.second].container.weight == weight)) {
        finalOrig.first = aboveCont+finalOrig.first;
        
    }
    //cout << "returned position: " << finalOrig.first << ", " << finalOrig.second << endl;
    return finalOrig;
}
State mainApp::siftProcedure() {
    vector<int> weightList;
    
    State currState = initState;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            if (currState.ship[i][j].status == USED) {
                weightList.push_back(currState.ship[i][j].container.weight);
            }
        }
    }
    sort(weightList.begin(), weightList.end(), greater<int>());
    
    pair<int,int> dest;
    int middleOfShip = 5;
    vector<pair<int,int>> destList;
    int siftGoal[8][12];

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            
            if (currState.ship[i][j].status != NOTEXIST) { // done
                siftGoal[i][j] = -1;
            }
            else {
                siftGoal[i][j] = -2;
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 12; j++) {
            
            if (destList.size() == weightList.size()) { // done
                i = 10;
                j = 10;
                break;
            }
            dest.first = i;
            if ((j%2) == 0) {
                middleOfShip -= j;
            }
            else {
                middleOfShip += j;
            }
            dest.second = middleOfShip;

            
            if (currState.ship[dest.first][dest.second].status == NOTEXIST) { //move to next row
                break;
            }
            else {
                //cout << "coords: " << dest.first << ", " << dest.second << endl;
                //cout << "destList.size : " << destList.size() << endl;
                siftGoal[dest.first][dest.second] = weightList.at(destList.size());
                //cout << siftGoal[dest.first][dest.second] << endl;
                destList.push_back(dest);
            }
        }
        middleOfShip =5;
    }
/*
    for (int i = 7; i > -1; i--) {
        for (int j = 0; j < 12; j++) {
            int check =siftCheck[i][j];
            cout << check << " ";
        }
        cout << endl;
    }*/

    //siftPlacement(weightList, destList, currState);
    bestState.emplace(initState);

    /*while(bestState.size() > 0){
        currState = bestState.top();
        bestState.pop();
        
    }*/
    int choosePos = 0;
    while (!destList.empty()) {
        int checkRow = destList.at(0).first;
        int choosePos = 0;
        for (choosePos = 0; choosePos < destList.size(); choosePos++) {
            if (checkRow != destList.at(choosePos).first) {
                choosePos = 0;
                break;
            }
            if (currState.ship[destList.at(choosePos).first][destList.at(choosePos).second].status == UNUSED) {
                break;
            }
        }
        pair<int,int> containerOrig = findNearestContainer(weightList.at(choosePos), destList.at(choosePos), currState);
        Container& currContainer = currState.ship[containerOrig.first][containerOrig.second].container;
        //cout << "Coordinates: " << destList.at(0).first << ", " << destList.at(0).second << " | currContainer: " << currContainer.description << ", "<< currContainer.weight << endl;
        //cout << "container currently located at " << currContainer.XY.first << ", " << currContainer.XY.second << endl;

        if (currContainer.XY == destList.at(choosePos)) {
            weightList.erase(weightList.begin() + choosePos);
            destList.erase(destList.begin() + choosePos);
            currState.ship[currContainer.XY.first][currContainer.XY.second].container.siftPlaced = true;
            currContainer.siftPlaced = true;
            continue;
        }
        else if(currState.ship[currContainer.XY.first][currContainer.XY.second].container.numContainerAbove > 0){
            //std::cout <<"Container found above our desired one" << std::endl;
            int row = currState.numOfcontainerInColumn[currContainer.XY.second].first-1;
            int column = currContainer.XY.second;
            Container& topContainer = currState.ship[row][column].container;
            int top1 = topContainer.XY.first;
            int top2 = topContainer.XY.second;
            pair<int,int> oldXY = topContainer.XY;
            int timeTrack = 99999;
            //std::cout <<"Need to move container at :" <<  top1 << ", " << top2 << std::endl;
            State dummyState = currState;
            checkRow = destList.at(0).first;
            for (int i = 0; i < destList.size(); i++) {
                if (checkRow != destList.at(0).first) {
                    break;
                }
                pair<int, int> checkCoords = findNearestContainer(weightList.at(i), destList.at(i), currState);
                int topCost = calculateTime(checkCoords, destList.at(i));
                if (checkCoords == topContainer.XY && topCost < timeTrack && currState.ship[checkCoords.first][checkCoords.second].status != USED) {
                    moveContainerBalance(checkCoords.second,topContainer,currState,0);
                    timeTrack = topCost;
                }
            }
            
            if (timeTrack != 99999) {
                //cout << "found other area" << endl;
                currState = dummyState;
            }
            else {
            for(int j = 0; j < COLUMN_SHIP; j++){
                
                // not in the same column
                if(j != destList.at(choosePos).second && j != currContainer.XY.second){
                    State newState;
                    
                    int emptyColumn = calculateEmptyColumn(currState,currContainer.XY.second);
                    if(emptyColumn == -2){
                        newState = currState;
                        //std::cout <<"No columns available, need to use buffer" << std::endl;
                        moveContainerBalance(emptyColumn,topContainer,newState,-1);
                        //bestState.emplace(newState);
                        currState = newState;
                        break;
                    }
                        // found a column that we can put on
                    else if(emptyColumn != -1 && emptyColumn != currContainer.XY.second && emptyColumn != destList.at(choosePos).second){
                        //dummyState = currState;
                        //std::cout <<"Found a column to place the top container at :" << emptyColumn << std::endl;
                        //cout << "currContainer.second : " << currContainer.XY.second << endl;
                        //cout << "destList.at(0).second : " << destList.at(0).second << endl;
                        moveContainerBalance(emptyColumn, topContainer, dummyState,0);
                        //cout << "new coords :" << topContainer.XY.first << ", " << topContainer.XY.second << endl;
                        /*if (calculateTime(oldXY, topContainer.XY) < timeTrack ) {
                            currState = dummyState;
                            timeTrack = calculateTime(oldXY, topContainer.XY);
                        } 
                        else if (calculateTime(oldXY, topContainer.XY) == timeTrack) {
                            if (topContainer.XY.second <= oldXY.second) {
                                currState = dummyState;
                            }
                        }*/
                        currState = dummyState;
                        //currState = newState;
                        break;
                    }
                    else{
                        //cout << "moving containers to column " << j << endl;
                        moveContainerBalance(j, topContainer,newState,0);
                        //bestState.emplace(newState);
                        currState = newState;
                    }
                }
            }
            }
            //currState = dummyState;
        }
        else if (currState.ship[destList.at(choosePos).first][destList.at(choosePos).second].status == USED) {
            
                //std::cout <<"Container found at or above destination" << std::endl;
            int row = currState.numOfcontainerInColumn[destList.at(choosePos).second].first-1;
            int column = destList.at(choosePos).second;
            Container& topContainer = currState.ship[row][column].container;
            pair<int, int> oldXY = topContainer.XY;
                //cout << "Have to move container at : " << row << ", " << column << endl;
                //cout << "Description: " << topContainer.description << " weight: " << topContainer.weight << endl;
                //std::cout <<"Need to move container at :" <<  top1 << ", " << top2 << std::endl;
            int timeTrack = 99999;
            //std::cout <<"Need to move container at :" <<  top1 << ", " << top2 << std::endl;
            State dummyState = currState;
            int checkRow = destList.at(0).first;
            for (int i = 0; i < destList.size(); i++) {
                if (checkRow != destList.at(0).first) {
                    break;
                }
                pair<int, int> checkCoords = findNearestContainer(weightList.at(i), destList.at(i), currState);
                int topCost = calculateTime(checkCoords, destList.at(i));
                if (checkCoords == topContainer.XY && topCost < timeTrack && currState.ship[checkCoords.first][checkCoords.second].status != USED) {
                    moveContainerBalance(checkCoords.second,topContainer,currState,0);
                    timeTrack = topCost;
                }
            }
            
            if (timeTrack != 99999) {
                currState = dummyState;
                //cout << "found other area" << endl;
            }
            else {
            for(int j = 0; j < COLUMN_SHIP; j++){
                State newState;
                    // not in the same column or column that 
                if(j != destList.at(choosePos).second && j != currContainer.XY.second){
                    newState = currState;
                    int emptyColumn = calculateEmptyColumn(currState,destList.at(choosePos).second);

                    if(emptyColumn == -2){
                            //std::cout <<"No columns available, need to use buffer" << std::endl;
                        moveContainerBalance(emptyColumn,topContainer,currState,-1);
                            //update coordinates in vector to buffer
                            //bestState.emplace(newState);
                            //currState = newState;
                        break;
                    }
                            // found a column that we can put on
                    else if(emptyColumn != -1 && emptyColumn != currContainer.XY.second && emptyColumn != destList.at(choosePos).second){
                            //std::cout << "Found a column to place the top container at :" << emptyColumn << std::endl;
                        moveContainerBalance(emptyColumn, topContainer, dummyState,0);
                            //cout << "new coords :" << topContainer.XY.first << ", " << topContainer.XY.second << endl;
                            //bestState.emplace(newState);
                            //currState = newState;
                        /*if (calculateTime(oldXY, topContainer.XY) < timeTrack ) {
                            currState = dummyState;
                            timeTrack = calculateTime(oldXY, topContainer.XY);
                        } 
                        else if (calculateTime(oldXY, topContainer.XY) == timeTrack) {
                            if (topContainer.XY.second <= oldXY.second) {
                                currState = dummyState;
                            }
                        }*/
                        currState = dummyState;
                        break;
                    }
                    else{
                            //cout << "moving containers to column " << j << endl;
                        moveContainerBalance(j, topContainer,currState,0);
                            //bestState.emplace(newState);
                            //currState = newState;
                    }
                    
                }
            }
            }
            
        }
        else{
            State newState;
            newState = currState;
            //std::cout <<"Moving container to column: " << destList.at(choosePos).second << std::endl;
            moveContainerBalance(destList.at(choosePos).second,currContainer, currState,0);
            //cout << "currCont : " << currContainer.XY.first << ", " << currContainer.XY.second << endl;
            //cout << "intended Dest: " << destList.at(choosePos).first << ", " << destList.at(choosePos).second << endl;
            weightList.erase(weightList.begin() + choosePos);
            destList.erase(destList.begin() + choosePos);
            currState.ship[currContainer.XY.first][currContainer.XY.second].container.siftPlaced = true;
            currContainer.siftPlaced = true;
            bestState.emplace(newState);
            //currState = newState;
        }
    }
    solutionState = currState;
    return currState;
}



//selects containers
stack<Container> mainApp::balanceList(State &currState) {
    Container currContainer;
    stack<Container> balance_list;

    float leftSideWeight = calculateLeftSideWeight(currState);
    float rightSideWeight = calculateRightSideWeight(currState);
    float differenceWeight = 99999;
    //currState.estRemainingCost = calcMisplaced(currState);
        
    if ((rightSideWeight/leftSideWeight) < 0.9) { //less weight on right side then left
        //std::cout << "less weight on right than left" << std::endl;
        for (int i = 7; i > -1; i--) {
            for (int j = 5; j > -1; j--) {
                //cout << "at " << i << ", " << j << endl;
                if (currState.ship[i][j].status == USED) {
                    float dif = abs((rightSideWeight + currState.ship[i][j].container.weight) - (leftSideWeight - currState.ship[i][j].container.weight));
                    //cout << "dif " << dif << endl; 
                    if ( dif < differenceWeight ) {
                        differenceWeight = dif;
                        currState.balanceDifference = dif;
                        currContainer = currState.ship[i][j].container;
                        while (!balance_list.empty()) {
                            //cout << "checking for pop" << endl;
                                if ((balance_list.top().weight == 0) || ((currContainer.weight/balance_list.top().weight) > 100) ) {
                                    //cout << "weight is 0" << endl;
                                    balance_list.pop();
                                }
                                else {
                                    break;
                                }
                               
                                 //cout << "done" << endl;
                        }
                        balance_list.push(currContainer);
			            //cout << "currContainer selected at : " << i << ", " << j << endl;

                        }
                    }
                }
            }
        }
        else if ((rightSideWeight/leftSideWeight) > 1.1) { // less weight on left than right
            //std::cout <<"less weight on left than right" << std::endl;
            for (int i = 7; i > -1; i--) {
                for (int j = 11; j > 5; j--) {
                    if (currState.ship[i][j].status == USED) {
                        float dif = abs((leftSideWeight + currState.ship[i][j].container.weight) - (rightSideWeight - currState.ship[i][j].container.weight));
                        if ( dif < differenceWeight ) {
                            differenceWeight = dif;
                            currState.balanceDifference = dif;
                            currContainer = currState.ship[i][j].container;
                            while (!balance_list.empty()) {
                                if ((balance_list.top().weight == 0) || ((currContainer.weight/balance_list.top().weight) > 100) ) {
                                    //cout << "weight is 0" << endl;
                                    balance_list.pop();
                                }
                                else {
                                    break;
                                }
                            }
                            balance_list.push(currContainer);
			               //cout << "currContainer selected at : " << i << ", " << j << endl;
                        }
                    }
                }
            }
        }
        //cout << "done with all" << endl;
        return balance_list;
}
void mainApp::balance_one(State &currState, Container curr) {
        
        Container currContainer = curr;
        float leftSideWeight = calculateLeftSideWeight(currState);
        float rightSideWeight = calculateRightSideWeight(currState);
        
        
	    int currCont1 = currContainer.XY.first;
	    int currCont2 = currContainer.XY.second;
        //std::cout <<"Container selected at :" << currCont1 << ", " << currCont2 << " Description: " << currContainer.description << " Weight: " << currContainer.weight << std::endl;
        
        if(currContainer.numContainerAbove > 0){
            //std::cout <<"Container found above our desired one" << std::endl;
            int row = currState.numOfcontainerInColumn[currContainer.XY.second].first-1;
            
            int column = currContainer.XY.second;
            Container topContainer = currState.ship[row][column].container;
	    int top1 = topContainer.XY.first;
	    int top2 = topContainer.XY.second;
            //std::cout <<"Need to move container at :" <<  top1 << ", " << top2 << std::endl;
            
            for(int i = 0; i < COLUMN_SHIP; i++){
                State newState;
                // not in the same column
                if(i != currContainer.XY.second){
                    newState = currState;
                    int emptyColumn = calculateEmptyColumn(currState,currContainer.XY.second);

                    if(emptyColumn == -2){
                        //std::cout <<"No columns available, need to use buffer" << std::endl;
                        moveContainerBalance(emptyColumn,topContainer,newState,-1);
                        bestStateBalance.emplace(newState);
                        break;
                    }
                    // found a column that we can put on
                    else if(emptyColumn != -1){
                        //std::cout <<"Found a column to place the top container at :" + emptyColumn << std::endl;
                        moveContainerBalance(emptyColumn, topContainer, newState,0);
                        bestStateBalance.emplace(newState);
                        break;
                    }
                    else{
                        moveContainerBalance(i, topContainer,newState,0);
                        bestStateBalance.emplace(newState);
                    }
                }
            }

        }
        else{
            State newState;
            newState = currState;
            int destCol = -1;
            if ((rightSideWeight/leftSideWeight) < 0.9) {
                destCol = 6;
                for (int i = 0; i < 8; i++) {
                    for (int j = 6; j < 12; j++) {
                        if (currState.ship[i][j].status == UNUSED) {
                            destCol = j;
                            break;
                        }
                    }
                }
            }
            else if ((rightSideWeight/leftSideWeight) > 1.1) {
                destCol = 5;
                for (int i = 0; i < 8; i++) {
                    for (int j = 5; j > -1; j--) {
                        if (currState.ship[i][j].status == UNUSED) {
                            destCol = j;
                            break;
                        }
                    }
                }
            }
            
            //std::cout <<"Moving container to column: " << destCol << std::endl;
            moveContainerBalance(destCol,currContainer, newState,0);

            bestStateBalance.emplace(newState);

        }
    }

bool mainApp::moveContainerBalance(int destColumn, Container &container, State &currState, int status){ //can be recursive 
        if ((container.numContainerAbove > 0) && (status != 3)) {
            //std::cout <<"Other containers are above the selected one: " << std::endl;
            /*
            for (int i = 7; i > container.XY.first; i--) {
                if (currState.ship[i][container.XY.second].status == USED) {
                    container = currState.ship[i][container.XY.second].container;
                    break;for (int i = 7; i > -1; i--) {
                for (int j = 0; j < 12; j++) {
                    if (currState.ship[i][j].status == UNUSED) {
                        cout << "000" << " | ";
                    }
                    else if (currState.ship[i][j].status == NOTEXIST) {
                        cout << "---" << " | ";
                    }
                    else {
                        cout << currState.ship[i][j].container.description << " | ";
                    }
                }
                cout << endl;
            }
            
            cout << "_______________________________________"<<endl;

            for (int i = 7; i > -1; i--) {
                        for (int j = 0; j < 12; j++) {
                                if (currState.ship[i][j].status == USED) {
                                        cout << currState.ship[i][j].container.weight << " | ";
                                }
                                else if (currState.ship[i][j].status == NOTEXIST) {
                                        cout << "----" << " | ";
                                }
                                else {
                                    cout << "UNUSED" << " | ";
                                }
                        }
                        cout << endl;
                }
                }
            }
            */
        }
        
        if ((container.XY.second > -1) && (status == 3)) {
            //cout << "wait how? " << endl;
            //cout << "old container xy: " << container.XY.first << ", " << container.XY.second << endl;
            container.XY.second = -1;

        }
        pair<int,int> dest;
        
        if (destColumn > -1) {
            dest.first = currState.numOfcontainerInColumn[destColumn].first;
            dest.second = destColumn;
        }
        
        pair<int,int> orig = container.XY;
        int timeToMoveCrane = 0;
        bool moved = true;
        pair<int,int> cranePos = currState.craneLocation;
      
        int timeToMove = 0;
       
        // ship to buffer
        if(status == -1){
            //cout << "Inside status -1" << endl;
            timeToMoveCrane += calculateTime(orig, cranePos);
            timeToMove += calculateTime(orig,pinkCell) + TIME_FROM_SHIP_TO_BUFFER;
            currState.craneLocation = pinkCellBuffer;
            currState.craneState = BUFFER;
            currState.ship[container.XY.first][container.XY.second].status = UNUSED;
            
            addMoveOrder(currState,orig,pinkCell,pinkCell);
            moveToBufferBalance(currState,container);

            updateNumContainerAbove(orig.second,-1,currState);
        }
        // within ship
        else if(status == 0){
            //cout << "Inside status 0" << endl;
            // check if column we are moving to has enough space
            if(currState.numOfcontainerInColumn[destColumn].first < ROW_SHIP){
                //cout << "container being moved" << container.XY.first << ", " << container.XY.second << endl;
                //cout << "dest" << dest.first << ", " << dest.second << endl;
                
                pair<int,int> highestColumnBetween = findHighestColumnBetween(dest,orig,currState);
                //cout << "highestColumnBetween: " << highestColumnBetween.first << ", " << highestColumnBetween.second << endl;
                timeToMove += calculateTime(orig,dest);
                if(currState.craneState == SHIP){
                    timeToMoveCrane = calculateTime(cranePos,orig);
                }

                currState.craneState = SHIP;
                currState.craneLocation = dest;
                if (!((container.XY.first < 0) || (container.XY.second < 0))) {
                    currState.ship[container.XY.first][container.XY.second].status = UNUSED;
                }
                currState.ship[dest.first][dest.second].status = USED;
                currState.ship[dest.first][dest.second].container = container;
		        currState.ship[dest.first][dest.second].container.XY = dest;

                container.XY = dest;

                // update state
                updateNumContainerAbove(orig.second, -1, currState);
                updateNumContainerAbove(dest.second, 1, currState);
                /*for (int i = 0; i < 12; i++) {
                    cout << initState.numOfcontainerInColumn[i].first << ", " << initState.numOfcontainerInColumn[i].second << endl;
                    
                }
                cout << "________________________________" << endl;
                for (int i = 0; i < 12; i++) {
                    
                    cout << currState.numOfcontainerInColumn[i].first << ", " << currState.numOfcontainerInColumn[i].second << endl;
                }*/

                timeToMove += 2;

                addMoveOrder(currState,orig,highestColumnBetween,dest);
            }
            else{
                //cout << "need to move other containers" << endl;
                
                // move a container to the buffer
                Container topContainer;
                for (int i = 7; i > container.XY.first; i--) {
                    if (currState.ship[i][destColumn].status == USED) {
                        topContainer = currState.ship[i][destColumn].container;
                        return moveContainerBalance(-2, topContainer, currState, -1);
                        
                    }
                }
                moved = false;
                //cout << "failed to move (this shouldn't happen)" << endl;
            }
        }
        // buffer to ship 
        else if(status == 3){
            //cout << "Inside status 3" << endl;
            //cout <<  container.XY.first << ", " << container.XY.second << endl;
            if(currState.craneState == SHIP){
                currState.craneState = BUFFER;
                currState.time+=TIME_FROM_SHIP_TO_BUFFER;
                timeToMove+=calculateTime(cranePos,pinkCell);
            }
            timeToMove += calculateTime(pinkCellBuffer, container.XY);
            addMoveOrder(currState,/*currState.buffer.top().XY*/ container.XY,pinkCellBuffer,pinkCellBuffer); //edit made here
            int emptyColumn = calculateEmptyColumn(currState, -1);
            //cout << "container being moved" << container.XY.first << ", " << container.XY.second << endl;
            destColumn = emptyColumn;
            if (destColumn > -1) {
                dest.first = currState.numOfcontainerInColumn[destColumn].first;
                dest.second = destColumn;
            }
            //moveContainerBalance(emptyColumn,container,currState,0);
            if(currState.numOfcontainerInColumn[destColumn].first < ROW_SHIP){
                //cout << "container being moved" << container.XY.first << ", " << container.XY.second << endl;
                //cout << "dest" << dest.first << ", " << dest.second << endl;
                
                pair<int,int> highestColumnBetween = findHighestColumnBetween(dest,orig,currState);
               // cout << "highestColumnBetween: " << highestColumnBetween.first << ", " << highestColumnBetween.second << endl;
                timeToMove += calculateTime(orig,dest);
                if(currState.craneState == SHIP){
                    timeToMoveCrane = calculateTime(cranePos,orig);
                }

                currState.craneState = SHIP;
                
                currState.craneLocation = dest;
                //cout << "container being moved" << container.XY.first << ", " << container.XY.second << endl;
                if (!((container.XY.first < 0) || (container.XY.second < 0))) {
                    currState.ship[container.XY.first][container.XY.second].status = UNUSED;
                }
                currState.ship[dest.first][dest.second].status = USED;
                currState.ship[dest.first][dest.second].container = container;
		        currState.ship[dest.first][dest.second].container.XY = dest;
                container.XY = dest;
                // update state
                //updateNumContainerAbove(orig.second, -1, currState);
                updateNumContainerAbove(dest.second, 1, currState);
                
                //cout << "containers in column updated" << endl;
                /*for (int i = 0; i < 12; i++) {
                    cout << initState.numOfcontainerInColumn[i].first << ", " << initState.numOfcontainerInColumn[i].second << endl;
                    
                }
                cout << "________________________________" << endl;
                for (int i = 0; i < 12; i++) {
                    
                    cout << currState.numOfcontainerInColumn[i].first << ", " << currState.numOfcontainerInColumn[i].second << endl;
                }*/

                timeToMove += 2;

                addMoveOrder(currState,orig,highestColumnBetween,dest);
                
            }
            else{
                //cout << "need to move other containers" << endl;
                
                // move a container to the buffer
                Container topContainer;
                for (int i = 7; i > container.XY.first; i--) {
                    if (currState.ship[i][destColumn].status == USED) {
                        topContainer = currState.ship[i][destColumn].container;
                        return moveContainerBalance(-2, topContainer, currState, -1);
                        
                    }
                }
                moved = false;
                //cout << "failed to move (this shouldn't happen)" << endl;
            }
        }
    currState.time += timeToMove + timeToMoveCrane;
    currState.cost = currState.time;

    //cout << "Moved " << container.XY.first << ", " << container.XY.second << endl;
    return moved;
}

void mainApp::moveToBufferBalance(State &currState, Container container){

    if(currState.buffer.empty()){
        cout << "buffer is empty" << endl;
        container.XY.first = 0;
        container.XY.second = COLUMN_BUFFER-1;
        currState.buffer.push(container);
    }
    else{
        Container topContainer = currState.buffer.top();

        if(topContainer.XY.first+1 >= ROW_BUFFER){
            container.XY.first = 0;
            container.XY.second = topContainer.XY.second-1;
        }
        else{
            container.XY.first = topContainer.XY.first+1;
            container.XY.second = topContainer.XY.second;
        }
        currState.buffer.push(container);
    }
    //cout << "Buffer containers: " << currState.buffer.size() << endl;
    currState.craneLocation = container.XY;
    currState.time += calculateTime(pinkCellBuffer, container.XY);
    addMoveOrder(currState,pinkCellBuffer,pinkCellBuffer,currState.buffer.top().XY);
}


//------------balance end-------------------

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
    orig.first = currState.numOfcontainerInColumn[column].first-1;
    
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

   /* TCHAR path[MAX_PATH] = {0};

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

    addComments("Manifest " + manifestName + " was written to desktop, and a reminder pop-up is displayed");*/
}

int mainApp::numOfMovesRemain(){
    return solutionState.containerMoveOrder.size()-currMoveSequence;
}

void mainApp::removeContainer(Container& container, State& currState){
    vector<Container> containers;
    for(int i = 0; i < currState.hashMapForContainer[container.key].size(); i++){
        if(currState.hashMapForContainer[container.key][i].XY != container.XY){
            containers.push_back(currState.hashMapForContainer[container.key][i]);
        }
    }

    currState.hashMapForContainer[container.key] = containers;
}
