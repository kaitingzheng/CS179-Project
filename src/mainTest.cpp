
#include <stdio.h>
#include "../header/mainApp.h"

int main() {
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase4.txt");
	
    State state = main->balanceSearch();

    for (int i = 0; i < state.containerMoveOrder.size(); i++) {
	    std::cout << "New container to move" << std::endl;
	    for (int j = 0; j <state.containerMoveOrder[i].size(); j++) {
		    std::cout << "Move Action: " << state.containerMoveOrder[i][j].first << ", " << state.containerMoveOrder[i][j].second << std::endl;
	    }
    }

	std::cout << "Initial State" << std::endl;
    for (int i = 7; i > -1; i--) {
		    for (int j = 0; j < 12; j++) {
			    if (main->initState.ship[i][j].status == UNUSED) {
				    cout << "000" << " | ";
			    }
			    else if (main->initState.ship[i][j].status == NOTEXIST) {
				    cout << "---" << " | ";
			    }
			    else {
			    	cout << main->initState.ship[i][j].container.description << " | ";
			    }
		    }
		    cout << endl;
	    }
	cout << "_____________________________________________________" << endl;
	for (int i = 7; i > -1; i--) {
		    for (int j = 0; j < 12; j++) {
			    if (main->initState.ship[i][j].status == USED) {
				    cout << main->initState.ship[i][j].container.weight << " | ";
			    }
			    else if (main->initState.ship[i][j].status == NOTEXIST) {
				    cout << "---" << " | ";
			    }
			    else {
			    	cout << "UNUSED" << " | ";
			    }
		    }
		    cout << endl;
	    }

    std::cout << "Final State" << std::endl;
    for (int i = 7; i > -1; i--) {
		    for (int j = 0; j < 12; j++) {
			    if (state.ship[i][j].status == UNUSED) {
				    cout << "000" << " | ";
			    }
			    else if (state.ship[i][j].status == NOTEXIST) {
				    cout << "---" << " | ";
			    }
			    else {
			    	cout << state.ship[i][j].container.description << " | ";
			    }
		    }
		    cout << endl;
	    }
	cout << "_____________________________________________________" << endl;
	for (int i = 7; i > -1; i--) {
		    for (int j = 0; j < 12; j++) {
			    if (state.ship[i][j].status == USED) {
				    cout << state.ship[i][j].container.weight << " | ";
			    }
			    else if (state.ship[i][j].status == NOTEXIST) {
				    cout << "---" << " | ";
			    }
			    else {
			    	cout << "UNUSED" << " | ";
			    }
		    }
		    cout << endl;
	}
	cout << "Solution cost: " <<  state.cost << endl;
    return 0;
}
