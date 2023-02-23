#include <gtest/gtest.h>
#include "../header/mainApp.h"

TEST(LOAD_TEST, LOAD_ONE_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> load;

    Container container;
    container.description = "Bird";
    container.weight = 9;

    load.push_back(container);

    vector<string> unload;

    State state = main->unload_load(unload,load);

    EXPECT_EQ(state.ship[1][0].container.description, "Bird");

}

TEST(LOAD_TEST, LOAD_FIVE_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> load;

    Container container;
    container.description = "Bird";
    container.weight = 9;

    Container container2;
    container2.description = "Turtle";
    container2.weight = 9;

    Container container3;
    container3.description = "Fly";
    container3.weight = 9;

    Container container4;
    container4.description = "Man";
    container4.weight = 9;

    Container container5;
    container5.description = "Woman";
    container5.weight = 9;

    load.push_back(container);
    load.push_back(container2);
    load.push_back(container3);
    load.push_back(container4);
    load.push_back(container5);
    vector<string> unload;

    State state = main->unload_load(unload,load);

    EXPECT_EQ(state.ship[1][0].container.description, "Woman");
    EXPECT_EQ(state.ship[2][0].container.description, "Man");
    EXPECT_EQ(state.ship[3][0].container.description, "Fly");
    EXPECT_EQ(state.ship[4][0].container.description, "Turtle");
    EXPECT_EQ(state.ship[5][0].container.description, "Bird");
}

TEST(LOAD_TEST, LOAD_8_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> container;
    for(int i = 0; i < 8; i++){
        Container container1;
        container1.description = "Bird";
        container1.weight = 9;
        container.push_back(container1);
    }
    
    vector<string> unload;

    State state = main->unload_load(unload,container);
    

    // for(int i = 0; i < 8; i++){
    //     for(int j = 0; j < 12; j++){
    //         if(state.ship[i][j].status == USED){
    //             cout << "-------"<< endl;
    //             cout << i << " " << j << " " << state.ship[i][j].container.description << endl;
    //         }
    //     }
    // }

    EXPECT_EQ(state.ship[1][1].container.description, "Bird");
}

TEST(LOAD_TEST, LOAD_92_CONTAINER){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    vector<Container> container;
    for(int i = 0; i < 92; i++){
        Container container1;
        container1.description = "Bird";
        container1.weight = 9;
        container.push_back(container1);
    }
    
    vector<string> unload;

    State state = main->unload_load(unload,container);
    

    // for(int i = 0; i < 8; i++){
    //     for(int j = 0; j < 12; j++){
    //         if(state.ship[i][j].status == USED){
    //             cout << "-------"<< endl;
    //             cout << i << " " << j << " " << state.ship[i][j].container.description << endl;
    //         }
    //     }
    // }

    EXPECT_EQ(state.ship[1][1].container.description, "Bird");
    EXPECT_EQ(state.ship[1][11].container.description, "Bird");
}