
#include <stdio.h>
#include "../header/mainApp.h"

int main() {
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase9.txt");
	
    State state = main->balanceSearch();

	vector<pair<int,int>> List;
    for (int i = 0; i < state.containerMoveOrder.size(); i++) {
	    std::cout << "New container to move" << std::endl;
		List = main->getNextMoveSequence();
	    for (int j = 0; j < List.size(); j++) {
		    std::cout << "Move Action: " << /*state.containerMoveOrder[i][j].first*/ List[j].first << ", " << /*state.containerMoveOrder[i][j].second*/ List[j].second << std::endl;
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
			    	cout << "UN" << " | ";
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
			    	cout << "UN" << " | ";
			    }
		    }
		    cout << endl;
	}
	cout << "Solution cost: " <<  state.cost << endl;
	delete main;
    return 0;
}
