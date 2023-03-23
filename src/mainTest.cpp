
#include <stdio.h>
#include "../header/mainApp.h"

int main() {
    mainApp *main = new mainApp();
    main->newShip("../sample_manifest/ShipCase6.txt"); // change this part to test different ship cases
	
    State state = main->balanceSearch();

	vector<pair<int,int>> List;
	cout << "List of Moves" << endl;
    for (int i = 0; i < state.containerMoveOrder.size(); i++) {
	    std::cout << "Moving New Container" << std::endl;
		List = main->getNextMoveSequence();
		cout << " | ";
	    for (int j = 0; j < List.size(); j++) {
		    std::cout << List[j].first << ", " << List[j].second << " | ";
	    }
		cout << endl;
    }
	cout << "_____________________________________________________" << endl;

	std::cout << "Initial State" << std::endl;
    for (int i = 7; i > -1; i--) {
		cout << " | ";
		    for (int j = 0; j < 12; j++) {
			    if (main->initState.ship[i][j].status == UNUSED) {
				    cout << "00" << " | ";
			    }
			    else if (main->initState.ship[i][j].status == NOTEXIST) {
				    cout << "--" << " | ";
			    }
			    else {
			    	cout << main->initState.ship[i][j].container.description.substr(0, 2) << " | ";
			    }
		    }
		    cout << endl;
	    }
	cout << "_____________________________________________________" << endl;
	for (int i = 7; i > -1; i--) {
		cout << " | ";
		    for (int j = 0; j < 12; j++) {
			    if (main->initState.ship[i][j].status == USED) {
				    cout << setw(5) << setfill('0')<< main->initState.ship[i][j].container.weight << " | ";
			    }
			    else if (main->initState.ship[i][j].status == NOTEXIST) {
				    cout << "-----" << " | ";
			    }
			    else {
			    	cout << "UNUSE" << " | ";
			    }
		    }
		    cout << endl;
	    }

	cout << "Initial ship balance: " << main->calculateBalance(main->initState) << endl;
	cout << endl;

    std::cout << "Final State" << std::endl;
    for (int i = 7; i > -1; i--) {
		cout << " | ";
		    for (int j = 0; j < 12; j++) {
				//cout << " | ";
			    if (state.ship[i][j].status == UNUSED) {
				    cout << "00" << " | ";
			    }
			    else if (state.ship[i][j].status == NOTEXIST) {
				    cout << "--" << " | ";
			    }
			    else {
			    	cout << state.ship[i][j].container.description.substr(0, 2) << " | ";
			    }
		    }
		    cout << endl;
	    }
	cout << "_____________________________________________________________________________________" << endl;
	for (int i = 7; i > -1; i--) {
		cout << " | ";
		    for (int j = 0; j < 12; j++) {
				//cout << " | ";
			    if (state.ship[i][j].status == USED) {
				    cout << setw(5) << setfill('0') << state.ship[i][j].container.weight << " | ";
			    }
			    else if (state.ship[i][j].status == NOTEXIST) {
				    cout << "-----" << " | ";
			    }
			    else {
			    	cout << "UNUSE" << " | ";
			    }
		    }
		    cout << endl;
	}
	cout << "Solution ship balance: " << main->calculateBalance(state) << endl;
	cout << "Solution cost: " <<  state.cost << endl;
	delete main;
    return 0;
}
