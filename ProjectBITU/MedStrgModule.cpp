#include "MedStrgModule.h"
#include <iostream>
#include "Artwork.h"
#include "Auth.h"
#include "MedMgmntSub.h"
#include "StorageMgmntSub.h"
#include "PreMgmntSubModule.h"
#include "config.h"
#include "Verifier.h"
#include "SuperAdmin.h"

using namespace std;

extern Auth auth;
extern Artwork art;
extern Verifier verifier;
extern SuperAdmin admin;
extern config conf;
MedMgmntSub medMgmntSub;
StorageMgmntSub storageMgmntSub;
PreMgmntSubModule preMgmntSub;


//this function shows the menu for the medicine storage management module (MSMM)
//the menu include sub-module under this module such as medicine management, 
//storage management and prescription management
void MedStrgModule::nurseModuleMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("Medicines Storage Management Module (MSMM)");

	cout << "\x1B[96mSelect the sub-module you want to access\033[0m\n\n";

	cout << "1 - Medicines Management sub-module\n";
	cout << "2 - Storage Management sub-module\n";
	cout << "3 - Prescription Management sub-module\n";
	cout << "4 - Return to admin menu\n";
	cout << "5 - Log out\n\n";

	cout << "Please enter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again pls\033[0m\n";
		system("pause");
		nurseModuleMenu();
	}
	else {
		switch (option) {
		case 1:
			medMgmntSub.MedsMgmntMenu();
			break;
		case 2:
			storageMgmntSub.StorageMgmntMenu();
			break;
		case 3:
			preMgmntSub.preMgmntMenu();
			break;
		case 4:
			if (verifier.verifyUserAuthorization((conf.getCurrentUserID()))) {
				admin.superAdminMenu();
			}
			else {
				cout << "\x1B[93m\nYou are not authorize to access this page\033[0m\n";
				cout << "\x1B[93mRedirecting you to previous page\033[0m\n\n";
				system("pause");
				nurseModuleMenu();	
			}
			break;
		case 5:
			system("pause");
			if (!auth.logout()) {
				nurseModuleMenu();
			}
			break;
		default:
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			nurseModuleMenu();
		}
	}
}
