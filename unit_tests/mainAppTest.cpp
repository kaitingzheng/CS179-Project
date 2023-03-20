#include <gtest/gtest.h>
#include "../header/mainApp.h"

TEST(PARSE_MANIFEST, CONTAINER_NAN){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    EXPECT_EQ(main->getContainer(1,1).weight, -1);

}

TEST(PARSE_MANIFEST, CONTAINER_DESCIPTION){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    EXPECT_EQ(main->getContainer(0,1).description, "Cat");

}

TEST(PARSE_MANIFEST, CONTAINER_UNUSED){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    EXPECT_EQ(main->getContainer(0,3).weight, -1);

}

TEST(PARSE_MANIFEST, CONTAINER_DESCRIPTION_TEST2){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase4.txt");

    EXPECT_EQ(main->getContainer(5,4).description, "Owl");

}
