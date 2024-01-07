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
extern Misc misc;

//this function is called from superadmin menu superadmin.cpp within superAdminMenu() function
//this function is used to display staff management module menu
void StaffMngmtModule::staffMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main Module Menu/Staff Management Module");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Register new staff\n"; 
	cout << "2 - Update existing staff\n";
	cout << "3 - Remove existing staff\n";
	cout << "4 - View staff list\n";
	cout << "5 - Search existing staff\n"; //TODO
	cout << "6 - Go back to Main Module Menu\n"; 
	cout << "7 - Log out\n";

	cout << "\nEnter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		staffMenu();
	} else {

		if (option == 1) {
			cout << "\nRedirecting you to register new staff page\n";
			system("pause");
			staffRegistration();
		}
		else if (option == 2) {
			cout << "\nRedirecting you to modify staff page\n";
			system("pause");
			staffListView();
			string* userData = staffSearch();
			system("pause");
			staffUpdate(userData);
		}
		else if (option == 3) {
			cout << "\nRedirecting you to remove staff record page\n";
			system("pause");
			staffListView();
			string* userData = staffSearch();
			system("pause");
			staffRemove(userData);
		}
		else if (option == 4) {
			cout << "\nRedirecting you to Staff List\n";
			system("pause");
			staffListView();
			string* userData = staffSearch();
			staffNextAction(userData);
		}
		else if (option == 5) {
			staffGeneralSearch();
		}
		else if (option == 6) {
			cout << "\nRedirecting you to Main Module Menu\n";
			system("pause");
			admin.superAdminMenu();
		}
		else if (option == 7) {
			system("pause");
			if (!auth.logout()) {
				staffMenu();
			}
		}
		else {
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			staffMenu();
			option = 0;
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

	cout << "\x1B[94mYou are about to register new user, to avoid redundancy of user please provide the new user IC\033[0m\n";
	cout << "\x1B[94mPlease enter the IC number without '-' and using this format (010203040606)\033[0m\n\n";

	//keep looping until all condition are met
	do {
		cout << "Enter new user IC number : ";
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

	//check if user exist in DB
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
			system("pause");
			break;
		}
	} while (continueLoop);

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
	art.directoryArt("Main Module Menu/Staff Management Module/Select User");

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
		for (int i = 0; i < 5; i++) {table.addColumn(c[i]);}

		while (row = mysql_fetch_row(res)) {
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
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function to search user and display menu to edit or remove user
string* StaffMngmtModule::staffSearch(){

	string userID;
	cin.clear();
	cin.ignore(INT_MAX, '\n');

	do {
		cout << "Please enter user ID : ";
		cin >> userID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//define sql statement and connection state
		string query = "SELECT staffID, staffIC, staffName, staffPhone, staffRole FROM users "
			"WHERE staffID ='" + userID + "'";
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
				system("pause");
				staffListView();
				return {};
			}
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}

	} while (true);

	row = mysql_fetch_row(res);
	string* userData = new string[5];

	for (int i = 0; i < 5; i++) {
		userData[i] = row[i];
	}
	
	return userData;
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

	string* updatedData = userData;
	
	for (int i = 0; i < 5; i++) {

		if (i == 4) {
			updatedData[i] = misc.getRoleString(stoi(userData[i]));
		}
		else {
			updatedData[i] = userData[i];
		}
	}
	table.addRow(updatedData);
	cout << table.draw();

	int option = 0;
	boolean continueLoop = true;

	do {
		
		cout << "\n\x1B[94mSelect Column you want to edit\033[0m\n\n";
		cout << "1 - Staff IC\n";
		cout << "2 - Staff Name\n";
		cout << "3 - Staff Phone\n";
		cout << "4 - Return to staff menu\n\n";
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

		switch (option) {
		case 1:
			staffUpdateIC(userData);
			break;
		case 2:
			staffUpdateName(userData);
			break;
		case 3:
			staffUpdatePhone(userData);
			break;
		case 4:
			staffMenu();
			break;
		}

	} while (continueLoop);

	
}

//function to edit staff IC
void StaffMngmtModule::staffUpdateIC(string* userData){

	string staffIC;

	do {
		cout << "\x1B[94mPlease enter the IC number without '-' and using this format (010203040606)\033[0m\n\n";
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
	staffMenu();
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
	staffMenu();
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
	staffMenu();
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

	boolean continueLoop = true;

	do {
		char option;
		cout << "\x1B[33mAre you sure want to remove this staff\033[0m\n";
		cout << "\nEnter your choice (Y/N) : ";
		cin >> option;

		switch (option) {
		case 'Y': {
			string query = "DELETE FROM users WHERE staffID = '" + userData[0] + "'";
			const char* c = query.c_str();
			conState = mysql_query(connection, c);

			if (!conState) {
				cout << "\n\x1B[32mStaff has been successfully removed\033[0m\n";
				system("pause");
				staffMenu();
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
			staffMenu();
			break;
		default:
			cout << "\x1B[31m\nInvalid input, please try again\033[0m\n" << mysql_errno(connection) << endl;
			system("pause");
			staffRemove(userData);
		}
	} while (continueLoop);
}

//this function is called from staffmeny() function
//if the user pick list staff option and search for specific staff
//this function will ask what their next action
void StaffMngmtModule::staffNextAction(string* userData) {

	int choice = 0;
	
	cout << "\n\x1B[94mPlease select your next action\033[0m\n\n";
	cout << "1 - Update staff\n";
	cout << "2 - Remove staff\n";
	cout << "3 - Go back to staff menu\n";
	cout << "Enter your choice : ";
	cin >> choice;
	
	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		staffMenu();
	}
	else {
		switch (choice) {
		case 1:
			staffUpdate(userData);
			break;
		case 2:
			staffRemove(userData);
			break;
		case 3:
			staffMenu();
			break;
		}
	}
}

//This function is called from staffMenu() function
//where this function is for general search such as search by name, IC, phone number
void StaffMngmtModule::staffGeneralSearch(){

	staffListView();

	string searchProperty;

	cout << "\n\x1B[94mYou can search staff on database via their staffID, name, IC number or phone number\033[0m\n\n";
	cout << "Enter your staff properties : ";
	cin >> searchProperty;	

	string query =
		"SELECT staffID, staffIC, staffName, staffPhone, staffRole FROM users "
		"WHERE staffID LIKE '%" + searchProperty + "%' "
		"OR staffName LIKE '%" + searchProperty + "%' "
		"OR staffIC LIKE '%" + searchProperty + "%' "
		"OR staffPhone LIKE '%" + searchProperty + "%'";
	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	
	if (!conState) {
		res = mysql_store_result(connection);
		int num_fields = mysql_num_fields(res);

		if (num_fields > 0) {
			
			system("pause");
			system("cls");
			art.logoArt();
			art.directoryArt("Main Module Menu/Staff Management Module/Search User");

			clitable::Table table;
			clitable::Column c[5] = {
					clitable::Column("Staff ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
					clitable::Column("Staff IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
					clitable::Column("Staff Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 50, clitable::Column::NON_RESIZABLE),
					clitable::Column("Staff Phone",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
					clitable::Column("Role",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
			};

			for (int i = 0; i < 5; i++) table.addColumn(c[i]);

			while (row = mysql_fetch_row(res)) {
				vector<string> row_data; // Create vector to hold data

				// Copy each field from the MySQL row into the vector
				for (int i = 0; i < num_fields; i++) {

					//convert int type value from DB to readable string
					if (i == 4) {
						int roleValue = atoi(row[i]);
						row_data.push_back(misc.getRoleString(roleValue));
					}
					else {
						// Add other fields to the vector
						row_data.push_back(row[i] ? row[i] : "NULL");
					}
				}
				// Convert the vector to array 
				string* userData = &row_data[0];

				//add data to the table
				table.addRow(userData);
			}
			cout << table.draw() << endl;
			mysql_free_result(res);
		}
		else {
			cout << "\n\x1B[31mStaff doesn't exist within such search properties. Please try again\033[0m\n";
			system("pause");
			staffGeneralSearch();
		}
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}


	do {
		char option;
		cout << "\n\x1B[94mDo you found your staff properties?\033[0m\n\n";
		cout << "Enter your choice (Y/N) : ";

		cin >> option;

		switch (option) {
		case 'Y':
		case 'y':
			break;
		case 'N':
		case 'n':
			cout << "\n\x1B[94mRedirecting you back to search page\033[0m\n";
			system("pause");
			staffGeneralSearch();
			break;
		default:
			cout << "\x1B[31m\nInvalid input, please try again\033[0m\n" << mysql_errno(connection) << endl;
			system("pause");
			staffGeneralSearch();
		}
		break;
	} while (true);

	do {
		string staffID;
		cout << "\nPlease enter the staff ID : ";
		cin >> staffID;

		string query = "SELECT * FROM users WHERE staffID ='" + staffID + "'";
		const char* cu = query.c_str();
		conState = mysql_query(connection, cu);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {

				row = mysql_fetch_row(res);
				string* userData = new string[5];

				for (int i = 0; i < 5; i++) {
					userData[i] = row[i];
				}
				staffNextAction(userData);
				break;
			}
			else {
				cout << "\n\x1B[31mStaff doesn't exist. Please try again\033[0m\n";
				system("pause");
				staffGeneralSearch();
			}
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}

	} while (true);

}
