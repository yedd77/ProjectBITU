#include "staffMngmtModule.h"
#include <iostream>
#include <string>
#include "Artwork.h"
#include "SuperAdmin.h"
#include "Auth.h"
#include "Misc.h"
#include "table.h"

using namespace std;

extern Artwork art;	
extern SuperAdmin admin;
extern Auth auth;
Misc misc;

//function to display staff management module menu
void StaffMngmtModule::staffMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/Staff Management Module");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Register new staff\n";
	cout << "2 - View existing staff\n";
	cout << "3 - Go back to Main Module Menu\n";
	cout << "4 - Log out\n";

	cout << "\nEnter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		staffMenu();
	}
	else {
		switch (option) {
		case 1:
			cout << "\nRedirecting you to Staff Registration\n";
			system("pause");
			staffRegistration();
			break;
		case 2:
			cout << "\nRedirecting you to Staff List View\n";
			system("pause");
			staffListView();
			break;
		case 3:
			cout << "\nRedirecting you back to Main Menu\n";
			system("pause");
			admin.superAdminMenu();
			break;
		case 4:
			system("pause");
			if (!auth.logout()) {
				staffMenu();
			}
			break;
		}
	}
}

//function to handle user registration
void StaffMngmtModule::staffRegistration(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/Staff Management Module/Register New User");

	string userID, userIC, userName, userPhone;
	int userRole;

	cout << "You are about to register new user, to avoid redundancy of user please provide the new user IC\n";
	cout << "\x1B[94mPlease enter the IC number without '-' and using this format (010203040606)\033[0m\n\n";

	//keep looping until all condition are met
	do {
		cout << "Enter new user IC : ";
		cin >> userIC;

		//IC validation 
		if (userIC.length() != 12) {
			cout << "\x1B[33mUser's IC number are not complete\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			userIC.clear();
		}
		else if (!misc.isNumeric(userIC)) {
			cout << "\x1B[33mUser's IC number are invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			userIC.clear();
		}
		else {
			break;
		}
	} while (true);

	if (misc.checkUserExist(userIC)) {
		cout << "\n\x1B[33mUser already exist, redirecting you back to Staff Management Module\033[0m\n";
		system("pause");
		staffMenu();
	}

	cout << "User did not exist in database\n";
	cout << "\n\x1B[94mPlease provide more information\n\033[0m\n";


	do {
		cout << "Enter user's name : ";
		cin.ignore();
		getline(cin, userName);

		if (userName.length() > 128) {
			cout << "\n\x1B[33mUser's name was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			userName.clear();
		}
		else {
			break;
		}
	} while (true);

	do {
		cout << "Enter user phone number : ";
		cin >> userPhone;

		if (userPhone.length() > 13) {
			cout << "\n\x1B[33mUser's phone number was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			userPhone.clear();
		}
		else if (!misc.isNumeric(userPhone)) {
			cout << "\n\x1B[33mUser's phone number was invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			userPhone.clear();

		}
		else {
			break;
		}
	} while (true);

	bool continueLoop = true;
	do {
		int numOption;

		cout << "\n\x1B[94mPlease select user role\033[0m\n\n";
		cout << "1 - Superadmin\n";
		cout << "2 - Nurse\n";
		cout << "3 - Care taker\n";

		cout << "Please assign a role to the new user : ";
		cin >> numOption;

		switch (numOption) {
		case 1:
			userRole = 1;
			continueLoop = false;
			break;
		case 2:
			userRole = 2;
			continueLoop = false;
			break;
		case 3:
			userRole = 3;
			continueLoop = false;
			break;
		default:
			cout << "\n\x1B[33mInvalid input\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			numOption = 0;
		}
	} while (continueLoop);

	//TODO
	//retrieve last record from DB
	string query =
		"SELECT * FROM users "
		"ORDER BY staffID "
		"DESC LIMIT 1";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		;
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	//convert SQL array to normal string and pass the value to createID 
	string passedID = row[0];
	userID = misc.createID(passedID, "STF");

	string insertQuery =
		"INSERT INTO users"
		"(staffID,staffIC,staffName,staffPass,staffPhone,staffRole,isPassChanged) "
		"VALUES "
		"('" + userID + "', '" + userIC + "', '" + userName + "' , 'MyPassword@77' , '" + userPhone + "' ,'" + to_string(userRole) + "',0)";

	const char* queryC = insertQuery.c_str();
	conState = mysql_query(connection, queryC);

	//Check if the query has successfully execute
	if (!conState) {

		printf("\x1B[32mSuccessfully registered the user\033[0m\n");
		cout << "\nDirecting you to Staff Management Module (SMM)\n";
		system("pause");
		staffMenu();
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function to display staff list
void StaffMngmtModule::staffListView(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/Staff Management Module/View User");

	//initialize query and table
	string query = "SELECT * FROM users";
	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);
	clitable::Table table;

	//check query exec
	if (!conState) {

		res = mysql_store_result(connection);
		int num_fields = mysql_num_fields(res);

		//set table design and header
		clitable::Column c[5] = {
			clitable::Column("Staff ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 50, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Phone",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Role",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
		};

		//add column to the table
		for (int i = 0; i < 5; i++) {
			table.addColumn(c[i]);
		}

		while ((row = mysql_fetch_row(res))) {
			vector<string> row_data; // Create vector to hold data

			// Copy each field from the MySQL row into the vector
			for (int i = 0; i < num_fields; i++) {

				//exclude unwanted column from db (ex: password)
				if (i != 3 && i != 6) {

					//convert int type value from DB to readable string
					if (i == 5) {
						int roleValue = atoi(row[i]);
						row_data.push_back(misc.getRoleString(roleValue));
					}
					else {
						// Add other fields to the vector
						row_data.push_back(row[i] ? row[i] : "NULL");
					}
				}
			}
			// Convert the vector to array 
			string* userData = &row_data[0];

			//add data to the table
			table.addRow(userData);
		}
		cout << table.draw() << endl;
		mysql_free_result(res);

		//todo error
		bool continueLoop = true;

		do {
			int option = 0;
			cout << "\x1B[94mPlease enter your next action\033[0m\n\n";
			cout << "1 - Edit or remove user\n";
			cout << "2 - Previous page\n\n";
			cout << "Enter your choice : ";


			if (cin >> option) {
				switch (option) {
				case 1:
					continueLoop = false;
					option = 0;
					staffSearch();
					break;
				case 2:
					cout << "Redirecting you to previous page\n";
					continueLoop = false;
					option = 0;
					system("pause");
					staffMenu();
					break;
				}
			}
			else {
				continueLoop = true;
				option = 0;
				cout << "\n\x1B[31mInvalid input\033[0m\n";
				cout << "\x1B[31mPlease try again\033[0m\n\n";
				cin.clear();
				cin.ignore();
			}


		} while (continueLoop);
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function to search user and display menu to edit or remove user
void StaffMngmtModule::staffSearch(){

	string userID;
	cin.clear();
	cin.ignore(INT_MAX, '\n');

	do {
		cout << "Please enter user ID : ";
		cin >> userID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//define sql statement and connection state
		string query = "SELECT * FROM users WHERE staffID ='" + userID + "'";
		const char* cu = query.c_str();
		conState = mysql_query(connection, cu);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				cout << "\n\x1B[32mUser found\033[0m\n\n";
				break;
			}
			else {
				cout << "\n\x1B[31mUser doesn't exist. Please try again\033[0m\n";
			}
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}

	} while (true);

	row = mysql_fetch_row(res);
	string userData[5];
	boolean continueLoop = true;
	int option;
	int userDataIndex = 0;

	for (int i = 0; i < 7; i++) {
		if (row[i] != nullptr) {
			if (i == 3 || i == 6) {
				continue;
			}
			else {
				userData[userDataIndex] = row[i];
				userDataIndex++;
			}
		}
		else {
			userData[userDataIndex] = "NULL";
			userDataIndex++;
		}
	}

	do {
		cout << "\x1B[94mPlease select your next option\033[0m\n\n";

		cout << "1 - Edit this user\n";
		cout << "2 - Remove this user\n";
		cout << "3 - Go back to previous page\n";
		cout << "Enter your option : ";
		cin >> option;

		if (cin.fail()) {
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\n\x1B[31mInvalid input\033[0m\n";
			cout << "\x1B[31mPlease try again\033[0m\n\n";
			option = 0;
			continue;
		}

		switch (option) {
		case 1:
			//edit user
			continueLoop = false;
			system("pause");
			staffUpdate(userData);
			break;
		case 2:
			//remove user
			continueLoop = false;
			system("pause");
			staffRemove(userData);
			break;
		case 3:
			staffListView();
		}
	} while (continueLoop);
}

//function to display properties to update
void StaffMngmtModule::staffUpdate(string* userData){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/Staff Management Module/View User/Edit user");

	clitable::Table table;
	clitable::Column c[5] = {
			clitable::Column("Staff ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 50, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Phone",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Role",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) table.addColumn(c[i]);

	table.addRow(userData);
	std::cout << table.draw();

	int option = 0;
	boolean continueLoop = true;

	do {
		
		cout << "\n\x1B[94mSelect Column you want to edit\033[0m\n\n";
		cout << "1 - Staff IC\n";
		cout << "2 - Staff Name\n";
		cout << "3 - Staff Phone\n";
		cout << "4 - Role\n";
		cout << "5 - Previous page\n\n";
		cout << "Enter your choice : ";
		cin >> option;

		if (cin.fail()) {
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\x1B[31mInvalid input\033[0m\n";
			cout << "\x1B[31mPlease try again\033[0m\n\n";
			option = 0;
			continue;
		}
		else {
			break;
		}

	} while (continueLoop);

	switch (option) {
	case 1:
		staffUpdateIC(userData);
	case 2:
		staffUpdateName(userData);
	case 3:
		staffUpdatePhone(userData);
	case 4:
		staffUpdateRole(userData);
	case 5:
		staffListView();
	}
}

//function to edit staff IC
void StaffMngmtModule::staffUpdateIC(string* userData){

	string staffIC;

	do {
		cout << "\n\nCurrent staff IC : " << userData[1] << endl;
		cout << "New staff IC : ";
		cin >> staffIC;

		if (staffIC.length() != 12) {
			cout << "\n\x1B[33mStaff's IC number are not complete\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			staffIC.clear();
		}
		else if (!misc.isNumeric(staffIC)) {
			cout << "\n\x1B[33mStaff's IC number are invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			staffIC.clear();
		}
		else if (misc.checkUserExist(staffIC)) {
			cout << "\n\x1B[33mStaff already exist\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
		}
		else {
			break;
		}
	} while (true);

	string query =
		"UPDATE users SET "
		"staffIC ='" + staffIC + "'"
		"WHERE staffID ='" + userData[0] + "'";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	cout << "\n\x1B[32mStaff IC has been successfully updated\033[0m\n";
	system("pause");
	staffListView();
}

//function to edit staff name
void StaffMngmtModule::staffUpdateName(string* userData){

	string staffName;

	do {
		cout << "\n\nCurrent staff Name : " << userData[2] << endl;
		cout << "New staff Name : ";
		cin.ignore();
		getline(cin, staffName);

		if (staffName.length() > 128) {
			cout << "\n\x1B[33mStaff's name was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			staffName.clear();
		}
		else {
			break;
		}
	} while (true);

	string query =
		"UPDATE users SET "
		"staffName ='" + staffName + "'"
		"WHERE staffID ='" + userData[0] + "'";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	cout << "\n\x1B[32mStaff Name has been successfully updated\033[0m\n";
	system("pause");
	staffListView();
}

//function to edit staff phone
void StaffMngmtModule::staffUpdatePhone(string* userData){

	string staffPhone;

	do {
		cout << "\n\nCurrent staff phone number : " << userData[3] << endl;
		cout << "New staff phone number : ";
		cin >> staffPhone;

		if (staffPhone.length() > 13) {
			cout << "\n\x1B[33mStaff's phone number was too long\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			staffPhone.clear();
		}
		else if (!misc.isNumeric(staffPhone)) {
			cout << "\n\x1B[33mStaff's phone number was invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			staffPhone.clear();

		}
		else {
			break;
		}
	} while (true);

	string query =
		"UPDATE users SET "
		"staffPhone ='" + staffPhone + "'"
		"WHERE staffID ='" + userData[0] + "'";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	cout << "\n\x1B[32mStaff phone number has been successfully updated\033[0m\n";
	system("pause");
	staffListView();
}

//function to edit staff role
void StaffMngmtModule::staffUpdateRole(string* userData){

	string currentRole;
	int staffRole;

	switch (stoi(userData[4])) {
	case 0:
		currentRole = "Unassigned";
		break;
	case 1:
		currentRole = "superadmin";
		break;
	case 2:
		currentRole = "Nurse";
		break;
	case 3:
		currentRole = "Caretaker";
		break;
	}

	int option;
	bool continueLoop = true;

	do {
		cout << "\n\nCurrent staff Role : " << currentRole << endl;
		cout << "Please select user role\n\n";
		cout << "1 - Superadmin\n";
		cout << "2 - Nurse\n";
		cout << "3 - Care taker\n";

		cout << "Please select new role : ";
		cin >> option;


		switch (option) {
		case 1:
			staffRole = 1;
			continueLoop = false;
			cout << "debug";
			break;
		case 2:
			staffRole = 2;
			continueLoop = false;
			break;
		case 3:
			staffRole = 3;
			continueLoop = false;
			break;
		default:
			cout << "\n\x1B[33mInvalid input\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n\n";
			system("pause");
			staffUpdateRole(userData);
		}
	} while (continueLoop);

	string query =
		"UPDATE users SET "
		"staffRole =" + to_string(staffRole) + " WHERE staffID ='" + userData[0] + "'";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	cout << "\n\x1B[32mStaff Role has been successfully updated\033[0m\n";
	system("pause");
	staffListView();
}

//function to remove user
void StaffMngmtModule::staffRemove(string* userData){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/Staff Management Module/View User/Remove user");

	clitable::Table table;
	clitable::Column c[5] = {
			clitable::Column("Staff ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 50, clitable::Column::NON_RESIZABLE),
			clitable::Column("Staff Phone",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
			clitable::Column("Role",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) table.addColumn(c[i]);

	table.addRow(userData);
	std::cout << table.draw();

	char option;
	cout << "\x1B[33mAre you sure want to remove this staff\033[0m\n";
	cout << "\nEnter your choice (Y/N) : ";
	cin >> option;

	boolean continueLoop = true;

	do {
		switch (option) {
		case 'Y': {
			string query = "DELETE FROM users WHERE staffID = '" + userData[0] + "'";
			const char* c = query.c_str();
			conState = mysql_query(connection, c);

			if (!conState) {
				cout << "\n\x1B[32mStaff has been successfully removed\033[0m\n";
				system("pause");
				staffListView();
			}
			else {
				cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}
			break;
		}
		case 'N':
			cout << "Redirecting you to previous page\n";
			system("pause");
			staffListView();
			break;
		default:
			cout << "\x1B[31m\nInvalid input, please try again\033[0m\n" << mysql_errno(connection) << endl;
			break;
		}
	} while (continueLoop);
}