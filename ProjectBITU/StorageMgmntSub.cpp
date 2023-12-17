#include "StorageMgmntSub.h"
#include "Artwork.h"
#include "MedStrgModule.h"
#include "Auth.h"
#include <iostream>

using namespace std;

extern Artwork art;
extern Auth auth;
extern MedStrgModule medStrgModule;

//function to display the storage management sub-module menu
void StorageMgmntSub::StorageMgmntMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Storage Management sub-module");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Add new medicines batches\n";
	cout << "2 - View medicines batches\n";
	cout << "3 = Go back to previous menu\n";
	cout << "4 - Log out\n";

	cout << "\nPlease enter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		StorageMgmntMenu();
	}
	else {
		switch (option) {
		case 1:
			//StorageMgmntAdd();
			break;
		case 2:
			//StorageMgmntView();
			break;
		case 3:
			cout << "\nRedirecting you back to previous menu\n";
			system("pause");
			medStrgModule.nurseModuleMenu();
			break;
		case 4:
			system("pause");
			if (!auth.logout()) {
				   StorageMgmntMenu();
			}
			break;
		}
	}
}

