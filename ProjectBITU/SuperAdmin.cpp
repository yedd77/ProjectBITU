#include <iostream>
#include <string>
#include "Artwork.h"
#include "Auth.h"
#include "SuperAdmin.h"
#include "MedStrgModule.h"
#include "staffMngmtModule.h"

using namespace std;

extern Artwork art;
extern Auth auth;
StaffMngmtModule staffModule;
MedStrgModule meds;

void SuperAdmin::superAdminMenu() {

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu");

	cout << "\x1B[94mSelect the module you want to access\033[0m\n\n";

	cout << "1 - Meds Storage Management Module (MSMM)\n";
	cout << "2 - Resident Management Module (RMM)\n";
	cout << "3 - Staff Management Module (SMM)\n";
	cout << "4 - Log out\n\n";

	cout << "Please enter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again pls\033[0m\n";
		system("pause");
		superAdminMenu();
	}
	else {
		switch (option) {
		case 1:
			cout << "\nRedirecting you to Medicines Management Module\n";
			system("pause");
			meds.nurseModuleMenu(); 
			break;
		case 2:
			// RMM Module
			// TODO
			break;
		case 3:
			// SMM
			cout << "\nRedirecting you to Staff Management Module\n";
			system("pause");
			staffModule.staffMenu();
			break;
		case 4:
			system("pause");
			if (!auth.logout()) {
				superAdminMenu();
			}
			break;
		default:
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			superAdminMenu();
		}
	}
}