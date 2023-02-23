#include <gtest/gtest.h>
#include "../header/mainApp.h"

TEST(UNLOAD_TEST, UNLOAD_ONE_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> load;
    vector<string> unload;

    string container = "00099Cat";
    unload.push_back(container);

     State state = main->unload_load(unload,load);

     EXPECT_EQ(state.ship[0][1].status, UNUSED);
}

TEST(UNLOAD_TEST, UNLOAD_TWO_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> load;
    vector<string> unload;

    string container = "00099Cat";
    string container1 = "00100Dog";
    unload.push_back(container);
    unload.push_back(container1);

     State state = main->unload_load(unload,load);

    EXPECT_EQ(state.ship[0][1].status, UNUSED);
    EXPECT_EQ(state.ship[0][2].status, UNUSED);

}

TEST(UNLOAD_TEST, UNLOAD_ONE_CONTAINER_W_CONTAINER_ABOVE){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase6.txt");

    vector<Container> load;
    vector<string> unload;

    string container = "00099Cat";
    unload.push_back(container);

    State state = main->unload_load(unload,load);

    EXPECT_EQ(state.ship[0][1].status, UNUSED);
    EXPECT_EQ(state.ship[1][0].status, USED);
}

TEST(UNLOAD_TEST, UNLOAD_ONE_CONTAINER_W_MULT_CONTAINER_ABOVE){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase4.txt");

    vector<Container> load;
    vector<string> unload;

    string container = "02000Cat";
    unload.push_back(container);

    State state = main->unload_load(unload,load);

    EXPECT_EQ(state.ship[1][4].status, UNUSED);
    EXPECT_EQ(state.ship[1][3].status, USED);
}