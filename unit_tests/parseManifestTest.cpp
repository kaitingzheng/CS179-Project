#include <gtest/gtest.h>
#include "../header/mainApp.h"


TEST(NEW_SHIP, GET_SHIP_NAME){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    EXPECT_EQ(main->getShipName(), "ShipCase1");
}



