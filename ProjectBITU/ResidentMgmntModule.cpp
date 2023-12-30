#include "ResidentMgmntModule.h"
#include "Artwork.h"
#include <iostream>
#include "SuperAdmin.h"
#include "Auth.h"
#include "Misc.h"
#include <string>
#include "table.h"

using namespace std;
extern Artwork art;
extern SuperAdmin admin;
extern Auth auth;
extern Misc misc;

//this function is called when admin select resident management module
// automatically call residentMenu() function if the user is not admin
// this will show all the resident management module menu
void ResidentMgmntModule::residentMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/ Resident Management Module/Resident Management Menu");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Register New Resident\n";
	cout << "2 - View Existing Resident\n";
	cout << "3 - Go back to Main Module menu\n";
	cout << "4 - Log out\n";

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
		switch (option) {
		case 1:
			cout <<"\nRedirecting you to Resident Registration\n";
			system("pause");
			residentRegistration();
			break;
		case 2:
			cout << "\nRedirecting you to Resident List\n";
			system("pause");
			//residentListView();
			break;
		case 3:
			cout << "\nRedirecting you back to Main Menu\n";
			system("pause");
			admin.superAdminMenu();
			break;
		case 4:
			cout << "\nRedirectin you to log out page\n";
			system("pause");
			if (!auth.logout()) {
				residentMenu();
			}
			break;
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

	cout << "\x1B[94mYou are about to register new resident, to avoid redundancy of user please provide the new resident IC\033[0m\n";
	cout << "\x1B[94mPlease enter the IC number without '-' and using this format (010203040606)\033[0m\n\n";

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
	cout << "\n\x1B[94mPlease provide more information\n\033[0m\n";

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
		cout << "\x1B[94m\nPlease enter resident's room number as in this format Block-Floor-Room Number (A-1-7)\033[0m\n";
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
		cout << "Enter user phone number : ";
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

	//check if the staff exist in the database
	do {
		cout << "\x1B[94mPlease enter the staff ID that you want to assign to this resident\033[0m\n\n";
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
