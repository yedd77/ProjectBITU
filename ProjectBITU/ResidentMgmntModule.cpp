#include "ResidentMgmntModule.h"
#include "Artwork.h"
#include <iostream>
#include "SuperAdmin.h"
#include "Auth.h"
#include "Misc.h"
#include <string>
#include "table.h"
#include "config.h"
#include "Verifier.h"

using namespace std;
extern Artwork art;
extern SuperAdmin admin;
extern Auth auth;
extern Misc misc;
extern config conf;
extern Verifier verifier;

//this function is called when admin select resident management module
// automatically call residentMenu() function if the user is not admin
// this will show all the resident management module menu
void ResidentMgmntModule::residentMenu() {

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu");

	cout << "\x1B[96mPlease select your next action\033[0m\n\n";

	cout << "1 - Register New Resident\n";
	cout << "2 - Update exsitng resident\n";
	cout << "3 - Delete exsitng resident record\n";
	cout << "4 - View Existing Resident\n";
	cout << "5 - Search Resident\n"; 
	cout << "6 - Medication Schedule\n"; //TODO
	cout << "7 - Go back to Main Module menu\n"; //TODO

	cout << "\nEnter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\n\x1B[91mInvalid input! Please enter a number.\033[0m\n\n";
		system("pause");
		residentMenu();
	}
	else {

		if (option == 1) {
			cout << "\nRedirecting you to Resident Registration\n";
			system("pause");
			residentRegistration();
		}
		else if (option == 2) {
			cout << "\nRedirecting you to update resident page\n";
			system("pause");
			string resID = residentListView();
			updateResident(resID);

		}
		else if (option == 3) {
			cout << "\nRedirecting you to delete resident record page\n";
			system("pause");
			string resID = residentListView();
			removeResident(resID);
		}
		else if (option == 4) {
			cout << "\nRedirecting you to Resident List\n";
			system("pause");
			string resID = residentListView();
			selectNextActionRes(resID);
		}
		else if (option == 5) {
			cout << "\nRedirecting you to search resident page\n";
			system("pause");
			string resID = searchGeneralResident();
			selectNextActionRes(resID);
		}
		else if (option == 6) {
			scheduleMenu();
		}
		else if (option == 7) {
			if (verifier.verifyUserAuthorization((conf.getCurrentUserID()))) {
				admin.superAdminMenu();
			}
			else {
				cout << "\x1B[93m\nYou are not authorize to access this page\033[0m\n";
				cout << "\x1B[93mRedirecting you to previous page\033[0m\n\n";
				system("pause");
				residentMenu();
			}
		}
		else {
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			residentMenu();
		}
	}
}

//this function is called after a user select it from residentMenu()
//this function gonna handle the registration of new resident
void ResidentMgmntModule::residentRegistration(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Resident Registration");

	string resID, staffID, resIC, resName, resRoomNum, resGuardName, resGuardPhone;

	cout << "\x1B[96mYou are about to register new resident, to avoid redundancy of user please provide the new resident IC\033[0m\n";
	cout << "\x1B[96mPlease enter the IC number without '-' and using this format (010203040606)\033[0m\n\n";

	//keep looping until user enter a valid IC number
	do {
		cout << "Enter new resident IC number : ";
		cin >> resIC;

		//IC validation
		if (resIC.length() != 12) {
			cout << "\x1B[33mResident's IC number are not complete\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resIC.clear();
		}
		else if (!misc.isNumeric(resIC)) {
			cout << "\x1B[33mUser's IC number are invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resIC.clear();
		}
		else {
			break;
		}
	}while(true);

	//check if the resident already exist in the database
	if (misc.checkResidentExist(resIC)) {
		cout << "\n\x1B[33mResident already exist, redirecting you back to Resident Management Module\033[0m\n";
		system("pause");
		residentMenu();
	}

	cout << "User did not exist in the database\n";
	cout << "\n\x1B[96mPlease provide more information\n\033[0m\n";

	//resident name instance
	do {
		cout << "Enter resident's name : ";
		cin.ignore();
		getline(cin, resName);

		if (resName.length() > 128) {
			cout << "\n\x1B[33mResident's name was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resName.clear();
		}
		else {
			break;
		}
	} while (true);

	//resident ID instance
	string currentID;
	string getLastId = "SELECT * FROM residents "
		"ORDER BY resID "
		"DESC LIMIT 1";
	const char* qn = getLastId.c_str();
	conState = mysql_query(connection, qn);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		if (row) {
			currentID = row[0];
		}
		else {
			currentID = "0000";
		}
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	resID = misc.createID(currentID, "RST");


	//resident room number instance
	do {
		cout << "\x1B[96m\nPlease enter resident's room number as in this format Block-Floor-Room Number (A-1-7)\033[0m\n";
		cout << "Enter resident's room number : ";
		cin >> resRoomNum;

		if (misc.isValidRoomNum(resRoomNum)) {
			
			//check if the room is available
			string checkRoom = "SELECT COUNT(*) FROM residents WHERE resRoomNum = '" + resRoomNum + "'";
			const char* qn = checkRoom.c_str();
			conState = mysql_query(connection, qn);

			if (!conState) {
				res = mysql_store_result(connection);
				row = mysql_fetch_row(res);
				int roomCount = atoi(row[0]);
				mysql_free_result(res);

				if (roomCount != 0) {
					cout << "\n\x1B[33mRoom is not available\033[0m\n";
					cout << "\x1B[33mPlease try again\033[0m\n";
					resRoomNum.clear();
				}
				else {
					break;
				}
			}
			else {
				cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}
			resRoomNum.clear();
		}
	} while (true);

	//resident guardian name instance
	do {
		cout << "Enter resident guardian's name : ";
		cin.ignore();
		getline(cin, resGuardName);

		if (resGuardName.length() > 128) {
			cout << "\n\x1B[33mResident guardian's name was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resGuardName.clear();
		}
		else {
			break;
		}
	} while (true);

	//resident guardian phone instance
	do  {
		cout << "Enter guardian phone number : ";
		cin >> resGuardPhone;

		if (resGuardPhone.length() > 13) {
			cout << "\n\x1B[33mUser's phone number was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resGuardPhone.clear();
		}
		else if (!misc.isNumeric(resGuardPhone)) {
			cout << "\n\x1B[33mUser's phone number was invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resGuardPhone.clear();
		}
		else {
			break;
		}
	} while (true);

	//staff ID instance
	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Resident Registration//Assign staff");

	//get staff data from database
	string getStaff = "SELECT "
		"s.staffID, "
		"s.staffIC, "
		"s.staffName, "
		"s.staffPhone, "
		"COUNT(r.resID) AS residentCount "
		"FROM "
		"users s "
		"LEFT JOIN "
		"residents r ON s.staffID = r.staffID "
		"WHERE "
		"s.staffRole = 3 "
		"GROUP BY "
		"s.staffID, s.staffIC, s.staffName, s.staffPhone";

	const char* q = getStaff.c_str();
	conState = mysql_query(connection, q);
	clitable::Table staffTable;

	if (!conState) {
		res = mysql_store_result(connection);
		int num_fields = mysql_num_fields(res);

		//set table design and header
		clitable::Column c[5] = {
			clitable::Column("Staff ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 50, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Phone",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Assigned",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
		};

		//add column to table
		for (int i = 0; i < 5; i++) { staffTable.addColumn(c[i]); }

		//add row to table
		while (row = mysql_fetch_row(res)) {

			vector<string> rowTable; //create new vector to store row data
			for (int i=0; i< num_fields; i++){rowTable.push_back(row[i] ? row[i] : "NULL");} //push row data to vector 
			string* userData = &rowTable[0]; //create pointer to vector
			staffTable.addRow(userData); //add row to table
		}
		cout << staffTable.draw() << endl;
		mysql_free_result(res);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	//check if the staff exist in the database
	do {
		cout << "\x1B[96mPlease enter the staff ID that you want to assign to this resident\033[0m\n\n";
		cout << "Enter staff ID : ";
		cin >> staffID;

		string checkStaff = "SELECT COUNT(*) FROM users WHERE staffID = '" + staffID + "' AND staffRole = '3'" ;
		const char* x = checkStaff.c_str();
		conState = mysql_query(connection, x);

		if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			int staffCount = atoi(row[0]);
			mysql_free_result(res);

			if (staffCount != 0) {
				break;
			}
			else {
				cout << "\n\x1B[33mStaff ID is invalid\033[0m\n";
				cout << "\x1B[33mPlease try again\033[0m\n";
				staffID.clear();
			}
		}
		else {
			cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	} while (true);

	//insert data to database
	string insertResident = "INSERT INTO residents "
		"(resID, staffID, resIC, resName, resRoomNum, resGuardName, resGuardPhone) "
		"VALUES "
		"('" + resID + "', '" + staffID + "', '" + resIC + "', '" + resName + "', '" + resRoomNum + "', '" + resGuardName + "', '" + resGuardPhone + "')";
	const char* y = insertResident.c_str();
	conState = mysql_query(connection, y);

	if (!conState) {
		cout << "\n\x1B[32mResident successfully registered\033[0m\n";
		cout << "\x1B[32mRedirecting you back to Resident Management Module\033[0m\n";
		system("pause");
		residentMenu();
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//this function is called after a user select it from residentMenu()
//this function gonna display all the resident in the database
//and ask the user what they want to do with the resident
string ResidentMgmntModule::residentListView(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Resident List");

	string getResidentData = "SELECT r.resID, r.resIC, r.resName, u.staffName, r.resRoomNum "
		"FROM residents r "
		"JOIN users u ON r.staffID = u.staffID ";
	const char* q = getResidentData.c_str();
	conState = mysql_query(connection, q);

	clitable::Table residentTable;
	clitable::Column column[5] = {
		clitable::Column("Resident ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 30, clitable::Column::NON_RESIZABLE),
		clitable::Column("Staff Assigned",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 30, clitable::Column::NON_RESIZABLE),
		clitable::Column("Room  Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE)
	};

	for(int i=0; i<5; i++){residentTable.addColumn(column[i]);}

	if (!conState) {
		res = mysql_store_result(connection);
		
		while (row = mysql_fetch_row(res)) {
			vector<string> rowTable;
			for (int i = 0; i < 5; i++) { rowTable.push_back(row[i] ? row[i] : "NULL"); }
			string* userData = &rowTable[0];
			residentTable.addRow(userData);
		}
		mysql_free_result(res);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	cout << residentTable.draw() << endl; 

	string resID;
	do {
		cout << "\n\x1B[96mPlease select the resident you want to select\033[0m\n";
		cout << "Enter resident ID : ";
		cin >> resID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//check if the resident exist in the database
		string checkResident = "SELECT COUNT(*) FROM residents WHERE resID = '" + resID + "'";
		const char* qn = checkResident.c_str();
		conState = mysql_query(connection, qn);
		
		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				cout << "\n\x1B[32mResident found\033[0m\n";
				break;
			}
		}
		else {
			cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}

	} while (true);

	return resID;
}

//this function is called from residentMenu() 
//after function residentListView() and residentSearch() is called where the user
//will be asked what to do with th selected resident
void ResidentMgmntModule::selectNextActionRes(string resID) {

	int option = 0;

	cout << "\n\x1B[96mPlease select your next action\033[0m\n\n";
	cout << "1 - Update Resident\n";
	cout << "2 - Delete Resident\n";
	cout << "3 - Go back to Resident menu\n\n";

	cout << "\nEnter your choice : ";
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		selectNextActionRes(resID);
	} 
	else {

		switch (option) {
		case 1:
			cout << "\nRedirecting you to update resident page\n";
			system("pause");
			updateResident(resID);
			break;
		case 2:
			cout << "\nRedirecting you to delete resident record page\n";
			system("pause");
			removeResident(resID);
			break;
		case 3:
			cout << "\nRedirecting you back to Resident menu\n";
			system("pause");
			residentMenu();
			break;
		default:
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			selectNextActionRes(resID);
			break;
		}
	}
}

//this function is called from residentMenu()
//user will be asked to enter the resident property that they want to search
string ResidentMgmntModule::searchGeneralResident(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Search resident");

	string getResidentData = "SELECT r.resID, r.resIC, r.resName, u.staffName, r.resRoomNum "
		"FROM residents r "
		"JOIN users u ON r.staffID = u.staffID ";
	const char* q = getResidentData.c_str();
	conState = mysql_query(connection, q);

	clitable::Table residentTable;
	clitable::Column column[5] = {
		clitable::Column("Resident ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 30, clitable::Column::NON_RESIZABLE),
		clitable::Column("Staff Assigned",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 30, clitable::Column::NON_RESIZABLE),
		clitable::Column("Room  Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) { residentTable.addColumn(column[i]); }

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector<string> rowTable;
			for (int i = 0; i < 5; i++) { rowTable.push_back(row[i] ? row[i] : "NULL"); }
			string* userData = &rowTable[0];
			residentTable.addRow(userData);
		}
		mysql_free_result(res);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	cout << residentTable.draw() << endl;

	string searchProperty;

	cout << "\n\x1B[96mYou can search resident from the database via their ID, name, IC number or room number\033[0m\n\n";
	cout << "Enter the resident property to search for : ";
	cin >> searchProperty;

	//check if the resident exist in the database
	string checkResident = "SELECT resID, resIC, resName, resRoomNum, resGuardName,	resGuardPhone "
		"FROM residents "
		"WHERE resID LIKE '%" + searchProperty + "%' "
		"OR resIC LIKE '%" + searchProperty + "%' "
		"OR resName LIKE '%" + searchProperty + "%' "
		"OR resRoomNum LIKE '%" + searchProperty + "%' "
		"OR resGuardName LIKE '%" + searchProperty + "%' "
		"OR resGuardPhone LIKE '%" + searchProperty + "%'";
	const char* cr = checkResident.c_str();
	conState = mysql_query(connection, cr);

	system("pause");
	if (!conState) {

		res = mysql_store_result(connection);
		int num_fields = mysql_num_fields(res);

		if (num_fields > 0) {

			system("cls");
			art.logoArt();
			art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Search resident");

			clitable::Table residentTable;
			clitable::Column c[6] = {
				clitable::Column("Resident ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
				clitable::Column("Resident IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE),
				clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 30, clitable::Column::NON_RESIZABLE),
				clitable::Column("Room Number",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
				clitable::Column("Guardian Name",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 20, clitable::Column::NON_RESIZABLE),
				clitable::Column("Guardian Phone",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 14, clitable::Column::NON_RESIZABLE)
			};

			for(int i = 0; i < 6; i++) { residentTable.addColumn(c[i]); }

			while (row = mysql_fetch_row(res)) {

				vector<string> rowTable;
				for (int i = 0; i < 6; i++) { rowTable.push_back(row[i] ? row[i] : "NULL"); }
				string* userData = &rowTable[0];
				residentTable.addRow(userData);
			}
			cout << residentTable.draw() << endl;
			mysql_free_result(res);
			
		}
		else {
			cout << "\n\x1B[31mResident doesn't exist within such search properties. Please try again\033[0m\n";
			system("pause");
			searchGeneralResident();
		}
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	
	do {
		char option;
		cout << "\n\x1B[96mDo you found your resident properties?\033[0m\n\n";
		cout << "Enter your choice (Y/N) : ";

		cin >> option;

		switch (option) {
		case 'Y':
		case 'y':
			break;
		case 'N':
		case 'n':
			cout << "\n\x1B[96mRedirecting you back to search page\033[0m\n";
			system("pause");
			searchGeneralResident();
			break;
		default:
			cout << "\x1B[31m\nInvalid input, please try again\033[0m\n" << mysql_errno(connection) << endl;
			system("pause");
			searchGeneralResident();
		}
		break;
	} while (true);

	do {
		string resID;
		cout << "\nSelect the resident ID that you want to select : ";
		cin >> resID;

		string checkID = "SELECT * FROM residents WHERE resID = '" + resID + "'";
		const char* ci = checkID.c_str();
		conState = mysql_query(connection, ci);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				return resID;
				break;
			}
			else {
				cout << "\n\x1B[31mResident doesn't exist within such search properties. Please try again\033[0m\n";
				system("pause");
				searchGeneralResident();
			}
		}
		else {
			cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}

	} while (true);
}

//this function is called after a user select it from residentListView()
//this function gonna display all the resident information
//this function going to show all update "able" information of the resident
//and ask the user what they want to update
void ResidentMgmntModule::updateResident(string resID){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Update Resident");

	//define sql query
	string getResidentData = "SELECT r.resID, r.resIC, r.resName, r.resRoomNum, r.resGuardName, resGuardPhone "
		"FROM residents r "
		"WHERE r.resID = '" + resID + "'";
	const char* q = getResidentData.c_str();
	conState = mysql_query(connection, q);

	clitable::Table residentTable;
	clitable::Column column[6] = {
		clitable::Column("Resident ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 24, clitable::Column::NON_RESIZABLE),
		clitable::Column("Room  Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 5, clitable::Column::NON_RESIZABLE),
		clitable::Column("Guardian Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 24, clitable::Column::NON_RESIZABLE),
		clitable::Column("Guardian Phone",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE)
	};

	for(int i=0; i<6; i++){residentTable.addColumn(column[i]);}

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		string residentData[6];
		for(int i =0; i<6; i++){residentData[i] = row[i] ? row[i] : "NULL";}
		residentTable.addRow(residentData);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	cout << residentTable.draw() << endl;
	
	boolean continueLoop = true;
	do {
		int option;
		cout << "\n\x1B[96mPlease select your next action\033[0m\n\n";
		cout << "1 - Update Resident Name\n";
		cout << "2 - Update Room Number\n";
		cout << "3 - Update Guardian Name\n";
		cout << "4 - Update Guardian Phone Number\n";
		cout << "5 - Go back to Resident Menu\n";
		cout << "\nEnter your choice : ";
		cin >> option;

		if (cin.fail()){
			cin.clear();
			cin.ignore();
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
		}
		else {
			switch (option) {
			case 1:
				updateResidentName(resID);
				continueLoop = false;
				break;
			case 2:
				updateResidentRoomNum(resID);
				continueLoop = false;
				break;
			case 3:
				updateGuardianName(resID);
				continueLoop = false;
				break;
			case 4:
				updateGuardianContact(resID);
				continueLoop = false;
				break;
			case 5 :
				cout << "\nRedirecting you back to Resident menu\n";
				system("pause");
				residentMenu();
				break;
			}
		}
	} while (continueLoop);
}

//this function is called after a user select it from updateResident()
//handle the update of resident name
void ResidentMgmntModule::updateResidentName(string resID){

	string resName;
	string getResidentData = "SELECT r.resName "
		"FROM residents r "
		"WHERE r.resID = '" + resID + "'";
	const char* q = getResidentData.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		resName = row[0];
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	do {
		cout << "\n\nCurrent resident name : " << resName;
		cout << "\n\n\x1B[96mPlease enter the new resident name\033[0m\n";
		cout << "Enter new resident name : ";
		cin.ignore();
		getline(cin, resName);

		if (resName.length() > 128) {
			cout << "\n\x1B[33mResident's name was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resName.clear();
		}
		else {
			break;
		}
	} while (true);

	string updateResidentName = "UPDATE residents "
		"SET resName = '" + resName + "' "
		"WHERE resID = '" + resID + "'";
	const char* x = updateResidentName.c_str();
	conState = mysql_query(connection, x);

	if (!conState) {
		cout << "\n\x1B[32mResident name successfully updated\033[0m\n";
		cout << "\x1B[32mRedirecting you back to Resident List\033[0m\n";
		system("pause");
		updateResident(resID);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//this function is called after a user select it from updateResident()
//handle the update of resident room number
void ResidentMgmntModule::updateResidentRoomNum(string resID){
	
	string resRoomNum;
	string getResidentData = "SELECT r.resRoomNum "
		"FROM residents r "
		"WHERE r.resID = '" + resID + "'";
	const char* q = getResidentData.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		resRoomNum = row[0];
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	do {
		cout << "\n\nCurrent resident room number : " << resRoomNum;
		cout << "\n\n\x1B[96m\nPlease enter resident's room number as in this format Block-Floor-Room Number (A-1-7)\033[0m\n";
		cout << "Enter resident's room number : ";
		cin >> resRoomNum;

		if (misc.isValidRoomNum(resRoomNum)) {

			//check if the room is available
			string checkRoom = "SELECT COUNT(*) FROM residents WHERE resRoomNum = '" + resRoomNum + "'";
			const char* qn = checkRoom.c_str();
			conState = mysql_query(connection, qn);

			if (!conState) {
				res = mysql_store_result(connection);
				row = mysql_fetch_row(res);
				int roomCount = atoi(row[0]);
				mysql_free_result(res);

				if (roomCount != 0) {
					cout << "\n\x1B[33mRoom is not available\033[0m\n";
					cout << "\x1B[33mPlease try again\033[0m\n";
					resRoomNum.clear();
				}
				else {
					break;
				}
			}
			else {
				cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}
			resRoomNum.clear();
		}
	} while (true);

	string updateResidentRoomNum = "UPDATE residents "
		"SET resRoomNum = '" + resRoomNum + "' "
		"WHERE resID = '" + resID + "'";
	const char* x = updateResidentRoomNum.c_str();
	conState = mysql_query(connection, x);
	
	if (!conState) {
		cout << "\n\x1B[32mResident room number successfully updated\033[0m\n";
		cout << "\x1B[32mRedirecting you back to Resident List\033[0m\n";
		system("pause");
		updateResident(resID);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//this function is called after a user select it from updateResident()
//handle the update of resident guardian name
void ResidentMgmntModule::updateGuardianName(string resID){
	
	string resGuardName;
	string getResidentData = "SELECT r.resGuardName "
		"FROM residents r "
		"WHERE r.resID = '" + resID + "'";
	const char* q = getResidentData.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		resGuardName = row[0];
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	do {
		cout << "\n\nCurrent resident guardian name : " << resGuardName;
		cout << "\n\n\x1B[96mPlease enter the new resident guardian name\033[0m\n";
		cout << "Enter new resident guardian name : ";
		cin.ignore();
		getline(cin, resGuardName);

		if (resGuardName.length() > 128) {
			cout << "\n\x1B[33mResident guardian's name was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resGuardName.clear();
		}
		else {
			break;
		}
	} while (true);

	string updateGuardianName = "UPDATE residents "
		"SET resGuardName = '" + resGuardName + "' "
		"WHERE resID = '" + resID + "'";
	const char* x = updateGuardianName.c_str();
	conState = mysql_query(connection, x);

	if (!conState) {
		cout << "\n\x1B[32mResident guardian name successfully updated\033[0m\n";
		cout << "\x1B[32mRedirecting you back to Resident List\033[0m\n";
		system("pause");
		updateResident(resID);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//this function is called after a user select it from updateResident()
//handle the update of resident guardian phone number
void ResidentMgmntModule::updateGuardianContact(string resID){

	string resGuardPhone;
	string getResidentData = "SELECT r.resGuardPhone "
		"FROM residents r "
		"WHERE r.resID = '" + resID + "'";
	const char* q = getResidentData.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		resGuardPhone = row[0];
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	do {
		cout << "\n\nCurrent resident guardian phone number : " << resGuardPhone;
		cout << "\n\n\x1B[96mPlease enter the new resident guardian phone number\033[0m\n";
		cout << "Enter new resident guardian phone number : ";
		cin >> resGuardPhone;

		if (resGuardPhone.length() > 13) {
			cout << "\n\x1B[33mResident guardian's phone number was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resGuardPhone.clear();
		}
		else if (!misc.isNumeric(resGuardPhone)) {
			cout << "\n\x1B[33mResident guardian's phone number was invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			resGuardPhone.clear();
		}
		else {
			break;
		}
	} while (true);

	string updateGuardianPhone = "UPDATE residents "
		"SET resGuardPhone = '" + resGuardPhone + "' "
		"WHERE resID = '" + resID + "'";
	const char* x = updateGuardianPhone.c_str();
	conState = mysql_query(connection, x);

	if (!conState) {
		cout << "\n\x1B[32mResident guardian phone number successfully updated\033[0m\n";
		cout << "\x1B[32mRedirecting you back to Resident List\033[0m\n";
		system("pause");
		updateResident(resID);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//this function is called after a user select it from residentListView()
//this function gonna handle the removal of resident
void ResidentMgmntModule::removeResident(string resID) {

	//confrimation
	do {

		cout << "\n\x1B[96mAre you sure you want to remove this resident?\033[0m\n";
		cout << "1 - Yes\n";
		cout << "2 - No\n";
		cout << "Enter your choice : ";

		int option;
		cin >> option;

		if (cin.fail()) {
			cin.clear();
			cin.ignore();
			cout << "\n\n\x1B[91mInvalid input! Please enter a number.\033[0m\n\n";
			system("pause");
			removeResident(resID);
		}
		else {
			switch (option) {
			case 1:
				break;
			case 2:
				cout << "\nRedirecting you back to Resident menu\n";
				system("pause");
				residentMenu();
				break;
			}

			string removeResident = "DELETE FROM residents "
				"WHERE resID = '" + resID + "'";
			const char* x = removeResident.c_str();
			conState = mysql_query(connection, x);

			if (!conState) {
				cout << "\n\x1B[32mResident successfully removed\033[0m\n";
				cout << "\x1B[32mRedirecting you back to Resident menu\033[0m\n";
				system("pause");
				residentMenu();
			}
			else {
				cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}
		}
	} while (true);
}

//this function is called from residentMenu and wil ask what scehdule
//the user want to view
void ResidentMgmntModule::scheduleMenu() {

	cout << "\n\x1B[96mPlease select schedule you want to view\033[0m\n\n";
	cout << "1 - Incomplete Schedule\n";
	cout << "2 - Complete Schedule\n";
	cout << "3 - Go back to Resident menu\n\n";

	int option;

	cout << "\nEnter your choice : ";
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		scheduleMenu();
	}
	else {

		switch (option) {
		case 1:
			cout << "\nRedirecting you to incomplete schedule page\n";
			system("pause");
			residentIncompleteSchedule();
			break;
		case 2:
			cout << "\nRedirecting you to complete schedule page\n";
			system("pause");
			residentCompleteSchedule();
			break;
		case 3:
			cout << "\nRedirecting you back to Resident menu\n";
			system("pause");
			residentMenu();
			break;
		default:
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			scheduleMenu();
			break;
		}
	}
}

//this function is called after a user select it from scheduleMenu()
//get userID and using it to find the resident's schedule that assigned to them
//if the user have incomplete schedule, it will show the incomplete schedule
//if user has completed all the schedule, it will show the completed schedule
void ResidentMgmntModule::residentIncompleteSchedule(){
	
	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Resident Schedule - Incomplete Schedule");
	//define query to find the assigned resident to the current logged in user
	string selectSchedule = "SELECT "
		"schedules.scheduleID, "
		"residents.resRoomNum, "
		"medicines.medsName, "
		"prescriptions.quantity, "
		"schedules.timeGiven, "
		"CASE "
		"WHEN schedules.isGiven = 1 THEN 'Given' "
		"WHEN schedules.isGiven = 0 THEN 'Not Given' "
		"END AS isGivenStatus "
		"FROM schedules "
		"JOIN prescriptions ON schedules.prescriptID = prescriptions.prescriptID "
		"JOIN residents ON prescriptions.resID = residents.resID "
		"JOIN users ON residents.staffID = users.staffID "
		"JOIN medicines ON prescriptions.medsID = medicines.medsID "
		"WHERE users.staffID = '" + conf.getCurrentUserID() + "' "
		"AND schedules.date = CURRENT_DATE "
		"AND schedules.isGiven = 0 "
		"ORDER BY schedules.timeGiven ASC ";
	const char* ss = selectSchedule.c_str();
	conState = mysql_query(connection, ss);
	
	if (verifier.verifyUserAuthorization(conf.getCurrentUserID())) {
		cout << "\n\x1B[31m\nYou are not authorized to view this page\033[0m\n";
		cout << "\x1B[31mRedirecting you back to Resident menu\033[0m\n";
		system("pause");
		residentMenu();
	}

	if (!conState) {
		res = mysql_store_result(connection);
		if (mysql_num_rows(res) < 1 ){
			cout << "\n\x1B[33mYou have no incoming schedule for today\033[0m\n";
			cout << "\x1B[33mRedirecting you to completed today schedule\033[0m\n";
			system("pause");   
			residentCompleteSchedule();
		}

		//create table
		clitable::Table scheduleTable;
		clitable::Column column[6] = {
			clitable::Column("Schedule ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE),
			clitable::Column("Room  Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
			clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 45, clitable::Column::NON_RESIZABLE),
			clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Time Given",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 10, clitable::Column::NON_RESIZABLE),
			clitable::Column("Status",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 10, clitable::Column::NON_RESIZABLE)
		};

		for(int i=0; i<6; i++){scheduleTable.addColumn(column[i]);}

		while (row = mysql_fetch_row(res)) {
			vector<string> rowTable;
			for (int i = 0; i < 6; i++) { rowTable.push_back(row[i] ? row[i] : "NULL"); }
			string* scheduleData = &rowTable[0];
			scheduleTable.addRow(scheduleData);
		}

		cout << scheduleTable.draw() << endl;
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	
	string scheduleID;
	do {

		cout << "\x1B[96mPlease enter the scheduleID of the prescription you have gave\033[0m\n\n";
		cout << "Enter schedule ID : ";
		cin >> scheduleID;

		//check if the scheduleID exist in the database
		string checkSchedule = "SELECT "
			"COUNT(*) AS prescriptionCount "
			"FROM "
			"schedules "
			"JOIN "
			"prescriptions ON schedules.prescriptID = prescriptions.prescriptID "
			"JOIN "
			"residents ON prescriptions.resID = residents.resID "
			"JOIN "
			"users ON residents.staffID = users.staffID "
			"JOIN "
			"medicines ON prescriptions.medsID = medicines.medsID "
			"WHERE "
			"users.staffID = '" + conf.getCurrentUserID() + "' "
			"AND schedules.date = CURRENT_DATE "
			"AND schedules.isGiven = 0 "
			"AND schedules.scheduleID = '" + scheduleID + "'";
		const char* cs = checkSchedule.c_str();
		conState = mysql_query(connection, cs);

		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		int scheduleCount = atoi(row[0]);
		mysql_free_result(res);

		if (scheduleCount != 0) {
			cout << "\n\x1B[32mSchedule Selected\033[0m\n\n";
			system("pause");
			break;
		}
		else {
			cout << "\n\x1B[33mSchedule ID is invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			scheduleID.clear();
		}
		
	} while (true);

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Resident Schedule - Record Schedule");

	string selectIndividualSchedule = "SELECT "
		"schedules.scheduleID, "
		"residents.resRoomNum, "
		"residents.resName, "
		"medicines.medsName, "
		"prescriptions.quantity, "
		"schedules.timeGiven "
		"FROM schedules "
		"JOIN prescriptions ON schedules.prescriptID = prescriptions.prescriptID "
		"JOIN residents ON prescriptions.resID = residents.resID "
		"JOIN users ON residents.staffID = users.staffID "
		"JOIN medicines ON prescriptions.medsID = medicines.medsID "
		"WHERE users.staffID = '" + conf.getCurrentUserID() + "' "
		"AND schedules.date = CURRENT_DATE "
		"AND schedules.scheduleID = '" + scheduleID + "'";
	const char* sis = selectIndividualSchedule.c_str();
	conState = mysql_query(connection, sis);

	res = mysql_store_result(connection);
	clitable::Table scheduleTable;
	clitable::Column column[6] = {
			clitable::Column("Schedule ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE),
			clitable::Column("Room  Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
			clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 30, clitable::Column::NON_RESIZABLE),
			clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 30, clitable::Column::NON_RESIZABLE),
			clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Time Given",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 10, clitable::Column::NON_RESIZABLE)
	};

	for(int i=0; i<6; i++){scheduleTable.addColumn(column[i]);}

	while (row = mysql_fetch_row(res)) {
		vector<string> rowTable;
		for (int i = 0; i < 6; i++) { rowTable.push_back(row[i] ? row[i] : "NULL"); }
		string* scheduleData = &rowTable[0];
		scheduleTable.addRow(scheduleData);
	}

	cout << scheduleTable.draw() << endl;

	do {
		char confirm;
		cout << "\n\x1B[96mYou are about to record this schedule as given\033[0m\n";
		cout << "\n\x1B[96mHave you gave the medicines to the resident?\033[0m\n";
		cout << "Enter Y to confirm or N to cancel : ";
		cin >> confirm;

		switch (confirm) {
		case 'Y':
		case 'y':
			updateSchedule(scheduleID);
			break;
		case 'N':
		case 'n':
			cout << "\nRedirecting you back to Resident Menu\n";
			system("pause");
			residentMenu();
			break;
		default:
			cout << "\n\x1B[33mInvalid input\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			system("pause");
			break;
		}

	} while (true);
	system("pause");
}

//this function is called if the user has completed all the schedule for the day
//called from residentIncompleteSchedule()
//this function gonna display all the schedule that the user has completed
void ResidentMgmntModule::residentCompleteSchedule(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu/Resident Schedule - Complete Schedule");

	string selectSchedule = "SELECT "
		"schedules.scheduleID, "
		"residents.resRoomNum, "
		"medicines.medsName, "
		"prescriptions.quantity, "
		"schedules.timeGiven, "
		"CASE "
		"WHEN schedules.isGiven = 1 THEN 'Given' "
		"WHEN schedules.isGiven = 0 THEN 'Not Given' "
		"END AS isGivenStatus "
		"FROM schedules "
		"JOIN prescriptions ON schedules.prescriptID = prescriptions.prescriptID "
		"JOIN residents ON prescriptions.resID = residents.resID "
		"JOIN users ON residents.staffID = users.staffID "
		"JOIN medicines ON prescriptions.medsID = medicines.medsID "
		"WHERE users.staffID = '" + conf.getCurrentUserID() + "' "
		"AND schedules.date = CURRENT_DATE "
		"AND schedules.isGiven = 1 "
		"ORDER BY schedules.timeGiven ASC ";

	const char* ss = selectSchedule.c_str();
	conState = mysql_query(connection, ss);

	if (!conState) {
		res = mysql_store_result(connection);

		//create table
		clitable::Table scheduleTable;
		clitable::Column column[6] = {
			clitable::Column("Schedule ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE),
			clitable::Column("Room  Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
			clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 45, clitable::Column::NON_RESIZABLE),
			clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Time Given",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 10, clitable::Column::NON_RESIZABLE),
			clitable::Column("Status",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 10, clitable::Column::NON_RESIZABLE)
		};

		for (int i = 0; i < 6; i++) { scheduleTable.addColumn(column[i]); }

		while (row = mysql_fetch_row(res)) {
			vector<string> rowTable;
			for (int i = 0; i < 6; i++) { rowTable.push_back(row[i] ? row[i] : "NULL"); }
			string* scheduleData = &rowTable[0];
			scheduleTable.addRow(scheduleData);
		}

		cout << scheduleTable.draw() << endl;
	}

	do {
		cout << "\n\x1B[96mPlease select your next action\033[0m\n\n";
		cout << "1 - Go back to Resident Management Menu\n";
		cout << "Enter your choice : ";

		int option;
		cin >> option;

		if (cin.fail()) {
			cin.clear();
			cin.ignore();
			cout << "\n\n\x1B[91mInvalid input! Please enter a number.\033[0m\n\n";
			system("pause");
			residentCompleteSchedule();
		}
		else {
			switch (option) {
			case 1:
				cout << "\nRedirecting you back to Resident Management Menu\n";
				system("pause");
				residentMenu();
				break;
			}
		}
	} while (true);
}

//this function is called after user confirm to record the schedule as given in residentIncompleteSchedule()
//this function gonna update the schedule status to given
void ResidentMgmntModule::updateSchedule(string sceheduleID){

	string updateSchedule = "UPDATE schedules "
		"SET isGiven = 1 "
		"WHERE scheduleID = '" + sceheduleID + "'";

	const char* us = updateSchedule.c_str();
	conState = mysql_query(connection, us);

	if (!conState) {
		updateMedsStock(sceheduleID);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
	}
}

//this function is revoked from updateMedsstock() where its called if the batch of the needed batch is not enough or none
//this function gonna rollback the update of schedule status
void ResidentMgmntModule::updateScheduleRollback(string sceheduleID){
	string updateSchedule = "UPDATE schedules "
		"SET isGiven = 0 "
		"WHERE scheduleID = '" + sceheduleID + "'";

	const char* us = updateSchedule.c_str();
	conState = mysql_query(connection, us);

	if (conState) {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
	}
}

//this function is called in updateSchedule()
//this function gonna update the medicine stock after the schedule is given
//this function going to get earlist expiry date first and select the quantity of prescribed medicine
//since there's many batches, this function gonna update the batch that have the earliest expiry date
void ResidentMgmntModule::updateMedsStock(string scheduleID){

	string oldQty, batchID, preQty;
	//select how the medicine quantity that the resident need to take,
	//select the batch ID that the batches have the earliest expiry date and need to be updated
	string selection = "SELECT "
		"prescriptions.quantity AS prescriptionQuantity, "
		"batches.batchQuantity AS currentStock, "
		"batches.batchID AS currentBatchID "
		"FROM batches "
		"JOIN medicines ON batches.medsID = medicines.medsID "
		"JOIN prescriptions ON medicines.medsID = prescriptions.medsID "
		"JOIN schedules ON prescriptions.prescriptID = schedules.prescriptID "
		"WHERE schedules.scheduleID = '" +scheduleID+ "' "
		"AND batchExpiry > CURDATE()"
		"ORDER BY batchExpiry ASC "
		"LIMIT 1";
	const char* s = selection.c_str();
	conState = mysql_query(connection, s);

	if (!conState) {
		res = mysql_store_result(connection);
		if (mysql_num_rows(res) == 0) {
			cout << "\n\x1B[33mThere's no more stock for this medicine\033[0m\n";
			cout << "\x1B[33mPlease contact the pharmacy to restock\033[0m\n";
			cout << "Redirecting you back to Resident Menu\n";
			updateScheduleRollback(scheduleID);
			system("pause");
			residentMenu();
		}
		row = mysql_fetch_row(res);
		preQty = row[0];
		oldQty = row[1];
		batchID = row[2];
	}
	else{
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
	}

	//calculate new batch quantity after the meds is given
	string newQty = to_string(stoi(oldQty) - stoi(preQty));

	//update new batch quantity
	string updateBatches = "UPDATE batches "
		"SET batchQuantity = '" + newQty + "' "
		"WHERE batchID = '" + batchID + "'";
	const char* ub = updateBatches.c_str();
	conState = mysql_query(connection, ub);
	if (!conState) {
		cout << "\n\x1B[32mSchedule successfully recorded as given\033[0m\n";
		cout << "\x1B[32mRedirecting you back to Resident Schedule - Incomplete Schedule\033[0m\n";
		system("pause");
		residentIncompleteSchedule();
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
	}

}