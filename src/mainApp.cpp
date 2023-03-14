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
                //cout << "At " << i << ", " << j << " : " << "Case 3" << endl;
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
        int column = currState.toBeUnloaded[i].XY.second;
        
        currState.toBeUnloaded[i].numContainerAbove = currState.ship[row][column].container.numContainerAbove;
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
    //cout << "Left in findHighestColumn: " << left << endl;
    //cout << "right in findHighestColumn: " << right << endl; 
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
    currState.cost = currState.time;



    return moved;
}

void mainApp::moveToBuffer(State &currState, Container container){
    //if (container.description == "NAN" || container.description == "")
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
            }
            // crane on truck then load
            else if(currState.craneState == TRUCK){
                load_one(currState);
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

State mainApp::balanceSearch(){ //balance
	
       bestState.emplace(initState);
	
        State currState;

        stack<Container> toBeBalanced;
        Container currCont;
        while(bestState.size() > 0){
            
            currState = bestState.top();
            bestState.pop();
            //----for debug purposes-------
            /*
            for (int i = 7; i > -1; i--) {
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
                }*/
            //-------

            float curBal = calculateBalance(currState);
            if( (curBal > 0.89) && (curBal < 1.11) && (currState.buffer.empty())){
                // found best solution
                std::cout << "goal reached" << std::endl;
                std::cout << calculateBalance(currState) << std::endl;
                std::cout << "goal cost: " << currState.cost << std::endl;
		solutionState = currState;
                return solutionState;
            }

            // balance
            else if((curBal < 0.9) || (curBal > 1.1)){ 
                toBeBalanced = balanceList(currState);
                
                while (!toBeBalanced.empty()) {
                    currCont = toBeBalanced.top();
                    toBeBalanced.pop();
                    balance_one(currState, currCont);
                }
                
            }

            // buffer to ship
            else if(!currState.buffer.empty()){
                Container currContainer = currState.buffer.top();
                currState.buffer.pop();
                moveContainerBalance(-1,currContainer,currState,3);
    
                bestState.emplace(currState);
            }

        }
        
        
        return initState;
    }
//selects containers
stack<Container> mainApp::balanceList(State &currState) {
    Container currContainer;
    stack<Container> balance_list;

    float leftSideWeight = calculateLeftSideWeight(currState);
    float rightSideWeight = calculateRightSideWeight(currState);
    float differenceWeight = 99999;
        
    if ((rightSideWeight/leftSideWeight) < 0.9) { //less weight on right side then left
        //std::cout << "less weight on right than left" << std::endl;
        for (int i = 7; i > -1; i--) {
            for (int j = 6; j > -1; j--) {
                if (currState.ship[i][j].status == USED) {
                    float dif = abs((rightSideWeight + currState.ship[i][j].container.weight) - (leftSideWeight - currState.ship[i][j].container.weight));
                    if ( dif < differenceWeight ) {
                        differenceWeight = dif;
                        currContainer = currState.ship[i][j].container;
                        if (!balance_list.empty()) {
                                if (balance_list.top().weight == 0) {
                                    balance_list.pop();
                                }
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
                            currContainer = currState.ship[i][j].container;
                            if (!balance_list.empty()) {
                                if (balance_list.top().weight == 0) {
                                    balance_list.pop();
                                }
                            }
                            balance_list.push(currContainer);
			               //cout << "currContainer selected at : " << i << ", " << j << endl;
                        }
                    }
                }
            }
        }
        return balance_list;
}
void mainApp::balance_one(State &currState, Container curr) {
        
        Container currContainer = curr;
        float leftSideWeight = calculateLeftSideWeight(currState);
        float rightSideWeight = calculateRightSideWeight(currState);
        
        
	    /*int currCont1 = currContainer.XY.first;
	    int currCont2 = currContainer.XY.second;
        std::cout <<"Container selected at :" << currCont1 << ", " << currCont2 << " Description: " << currContainer.description << " Weight: " << currContainer.weight << std::endl;
        */
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
                        bestState.emplace(newState);
                        break;
                    }
                    // found a column that we can put on
                    else if(emptyColumn != -1){
                        //std::cout <<"Found a column to place the top container at :" + emptyColumn << std::endl;
                        moveContainerBalance(emptyColumn, topContainer, newState,0);
                        bestState.emplace(newState);
                        break;
                    }
                    else{
                        moveContainerBalance(i, topContainer,newState,0);
                        bestState.emplace(newState);
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

            bestState.emplace(newState);

        }
    }

bool mainApp::moveContainerBalance(int destColumn, Container &container, State &currState, int status){ //can be recursive 
        if ((container.numContainerAbove > 0) && (status != 3)) {
            //std::cout <<"Other containers are above the selected one: " << std::endl;
            for (int i = 7; i > container.XY.first; i--) {
                if (currState.ship[i][container.XY.second].status == USED) {
                    container = currState.ship[i][container.XY.second].container;
                    break;
                }
            }
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
                /*if (currState.ship[5][0].status == UNUSED) {
                    cout << "check status of 5,0: UNUSED" << endl;
                }*/
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
    cout << "Buffer containers: " << currState.buffer.size() << endl;
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

    for(int i = 0; i < COLUMN_SHIP; i++){
        if(arr[i] == false && currState.numOfcontainerInColumn[i].first < ROW_SHIP && i != column){
            if(column == -1){
                return i;
            }
            else if(index == -1){
                index = i;
            }
            else if(abs(i - column) < abs(index - column)){
                index = i;
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
    /*std::cout << "Adding orig to moveOrder: " << orig.first << ", " << orig.second << std::endl;
    std::cout << "midPoint: " << midPoint.first << ", " << midPoint.second << std::endl;
    std::cout << "dest: " << dest.first << ", " << dest.second << std::endl;*/
    
    if(orig != pinkCell && dest != pinkCell){
        //std::cout << "Inside orig and dest != pinkCell" << std::endl;
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
                //std::cout << "Updated orig added into moveOrder: " << orig.first << ", " << orig.second << std::endl;

                moveOrder.push_back(orig);
        }
        currState.containerMoveOrder.push_back(moveOrder);
        return;
    }
    
    int limiter = 0;
    while(orig != midPoint){
        //std::cout << "Inside orig != midPoint" << std::endl;
        // check if mid point is above orig, if above then we have to go up and over
        if(midPoint.first > orig.first){
            //cout << "one" << endl;
            orig.first++;
        }
        // midpoint is on the left
        else if(orig.second > midPoint.second){
            //cout << "two" << endl;
            orig.second--;
        }
        // midpoing is on the right
        else if(orig.second < midPoint.second){
            //cout << "three" << endl;
            orig.second++;
        }
        else if(midPoint.first < orig.first) {
            orig.first--;
        }
        
        //std::cout << "Updated orig added into moveOrder: " << orig.first << ", " << orig.second << std::endl;
        moveOrder.push_back(orig);
    }

    while(orig != dest){
        //std::cout << "Inside orig != dest" << std::endl;

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
        //std::cout << "Updated orig added into moveOrder: " << orig.first << ", " << orig.second << std::endl;
        moveOrder.push_back(orig);
    }
    //std::cout << "moveOrder added into containerMoveOrder" << std::endl;
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
        //createManifest();
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
/*
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
}*/
