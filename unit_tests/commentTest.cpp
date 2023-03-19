#include <gtest/gtest.h>
#include "../header/mainApp.h"


TEST(COMMENT, ADD_COMMENT){
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase1.txt");

    string comment = "TEST";
    main->addComments(comment);
}