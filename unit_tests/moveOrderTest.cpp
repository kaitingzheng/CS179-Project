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

    loc.first = 8;
    loc.second = -1;

    EXPECT_EQ(state.containerMoveOrder[0][state.containerMoveOrder[0].size()-1], loc);
}