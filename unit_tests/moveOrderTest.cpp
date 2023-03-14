#include <gtest/gtest.h>
#include "../header/mainApp.h"

TEST(MOVEORDER_TEST, UNLOAD_ONE){
    
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> load;
    vector<string> unload;

    string container = "00099Cat";
    unload.push_back(container);

     State state = main->unload_load(unload,load);

     EXPECT_EQ(state.ship[0][1].status, UNUSED);
     EXPECT_EQ(state.containerMoveOrder.size(), 1);
    
    pair<int,int> loc;
    loc.first = 0;
    loc.second = 1;
    for(int i = 0; i < state.containerMoveOrder[0].size()-2; i++){
        EXPECT_EQ(state.containerMoveOrder[0][i], loc);
        loc.first++;
    }
    main->getNextMoveSequence();

    loc.first = 8;
    loc.second = -1;

    EXPECT_EQ(state.containerMoveOrder[0][state.containerMoveOrder[0].size()-1], loc);
}

// TEST(MOVEORDER_TEST, LOAD_ONE){
    
//     mainApp *main = new mainApp();
//     main->newShip("../sample_manifest/ShipCase2.txt");

//     vector<Container> load;
//     vector<string> unload;

//     Container container;
//     container.description = "bat";
//     container.weight = 431;
//     load.push_back(container);

//      State state = main->unload_load(unload,load);

//      EXPECT_EQ(state.ship[0][1].status, UNUSED);
//      EXPECT_EQ(state.containerMoveOrder.size(), 1);
    
//     pair<int,int> loc;
//     loc.first = 0;
//     loc.second = 1;
//     for(int i = 0; i < state.containerMoveOrder[0].size()-2; i++){
//         EXPECT_EQ(state.containerMoveOrder[0][i], loc);
//         loc.first++;
//     }
//     vector<pair<int,int>> moveOrder = main->getNextMoveSequence();
//     while(!moveOrder.empty()){
//         for(int i = 0; i < moveOrder.size(); i++){
//             cout << moveOrder[i].first << " " <<  moveOrder[i].second << endl;
//         }
//         moveOrder = main->getNextMoveSequence();
//     }

//     loc.first = 8;
//     loc.second = -1;

    
// }

// TEST(CALCULATE_HIGHEST_COLUMN, ZERO){
//     mainApp *main = new mainApp();
//     main->newShip("../sample_manifest/ShipCase2.txt");
//     pair<int,int> orig;
//     orig.first = 0;
//     orig.second = 5;

//     pair<int,int> dest;
//     dest.first = 0;
//     dest.second = 8;
    
//     pair<int,int> sol = main->findHighestColumnBetween(orig,dest,main->initState);
//     cout << sol.first << " " << sol.second << endl;
// }