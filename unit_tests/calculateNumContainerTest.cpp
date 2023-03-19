#include <gtest/gtest.h>
#include "../header/mainApp.h"


TEST(CALCULATE_NUM_CONTAINER, NO_CONTAINER_ON_TOP){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");
    EXPECT_EQ(main->getContainer(0,1).numContainerAbove, 0);
}

TEST(CALCULATE_NUM_CONTAINER, ONE_CONTAINER_ON_TOP){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase6.txt");
    EXPECT_EQ(main->getContainer(0,1).numContainerAbove, 1);
}

TEST(CALCULATE_NUM_CONTAINER, NO_CONTAINER_ON_EMPTY_COLUMN){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase6.txt");
    EXPECT_EQ(main->getContainer(0,0).numContainerAbove, 0);
}

TEST(CALCULATE_NUM_CONTAINER, FOUR_CONTAINER_ON_TOP){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase4.txt");
    EXPECT_EQ(main->getContainer(3,4).numContainerAbove, 4);
}