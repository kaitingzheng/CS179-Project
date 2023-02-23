#include <gtest/gtest.h>
#include "../header/mainApp.h"

TEST(LOAD_UNLOAD_TEST, UNLOAD_LOAD_ONE_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> load;
    vector<string> unload;

    string container = "00099Cat";
    unload.push_back(container);

    Container container1;
    container1.description = "Bird";
    container1.weight = 9;

    load.push_back(container1);

    State state = main->unload_load(unload,load);

    EXPECT_EQ(state.ship[0][1].status, UNUSED);
    EXPECT_EQ(state.ship[1][0].container.description, "Bird");
}


TEST(LOAD_UNLOAD_TEST, UNLOAD_ONE_LOAD_TWO_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase3.txt");

    vector<Container> load;
    vector<string> unload;

    string container = "00500Cow";
    unload.push_back(container);

    Container container1;
    container1.description = "Bat";
    container1.weight = 5432;

    Container container2;
    container2.description = "Rat";
    container2.weight = 5397;

    load.push_back(container1);
    load.push_back(container2);

    State state = main->unload_load(unload,load);

    EXPECT_EQ(state.ship[0][1].status, UNUSED);
    EXPECT_EQ(state.ship[2][0].status, USED);
    EXPECT_EQ(state.ship[3][0].status, USED);
}