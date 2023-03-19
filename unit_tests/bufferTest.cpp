#include <gtest/gtest.h>
#include "../header/mainApp.h"

TEST(BUFFER_TEST, MOVE_ONE_CONTAINER_TO_BUFFER_AND_BACK){
    
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase7.txt");

    vector<Container> load;

    vector<string> unload;
    string container = "00001Bird";

    unload.push_back(container);

    State state = main->unload_load(unload,load);

    // for(int i = 0; i < 8; i++){
    //     for(int j = 0; j < 12; j++){
    //         if(state.ship[i][j].status == USED){
    //             cout << "-------"<< endl;
    //             cout << i+1 << " " << j+1 << " " << state.ship[i][j].container.description << endl;
    //         }
    //     }
    // }

    EXPECT_EQ(state.ship[6][0].container.description, "Turtle");
    delete main;
}