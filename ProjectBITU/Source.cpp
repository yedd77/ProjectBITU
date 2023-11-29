#include <iostream>
#include <string>
#include <conio.h>
#include <mysql.h>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include "table.h"
#include <limits>
#include "Artwork.h"

using namespace std;

int conState;
MYSQL* connection;
MYSQL_ROW row;
MYSQL_RES* res;

void staffMngt();
void moduleSuperadmin();
void editUser(string* userData);
void staffMngtView();

Artwork art;

boolean connectionFunction() {

	art.logoArt();
	cout << "Initializing database connection";
	art.loadingArt();

	//initializing database connection
	connection = mysql_init(0);
	if (connection) {

		printf("\x1B[32mDatabase connection successfully initialized\033[0m\n\n");

		//establish a connection to database
		connection = mysql_real_connect(connection, "localhost", "root", "", "elderlyHome", 3306, NULL, 0);

		cout << "Establishing database connection";
		art.loadingArt();

		if (connection) {

			printf("\x1B[32mDatabase connection successfully established\033[0m\n");
			return true;
		}
		else {

			printf("\x1B[31mDatabase connection fails to establish\033[0m\n");
			return false;
		}
	}
	else {

		printf("\x1B[31mDatabase connection fails to initialize\033[0m\n");
		return false;
	}
}

//function to initialize the system if the system hasnt been initialize before
boolean firstTimeSetup() {
	
	//defining SQL query
	string query = "SELECT COUNT(*) FROM users WHERE staffID='STF0001'";
	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);
	
	//check query execution
	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		int userCount = atoi(row[0]);
		mysql_free_result(res);
		
		//check if the user exist
		if (userCount == 1) {
			
			printf("\x1B[32mSystem has been initialized\033[0m\n\n");
			return true;
		}
		else {

			printf("\x1B[33mSystem has not been initialized\033[0m\n\n");
			cout << "Initializing system";
			art.loadingArt();

			//Creating row in user table as superadmin with default password
			//TODO 1: change pass and userIC
			string query = "INSERT INTO users(staffID,staffIC,staffName,staffPass,staffPhone,staffRole,isPassChanged) VALUES ('STF0001' , '123' , 'Super Admin' , '123' , '000000000000' , 1 , 0 )";
			const char* queryC = query.c_str();
			conState = mysql_query(connection, queryC);
			
			//Check if the query has successfully execute
			if (!conState) {

				printf("\x1B[32mSystem initialized\033[0m\n");
				return true;
				
			}
			else {
				cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
				return false;
			}
		}
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		return false;
	}
}

boolean loggingOut() {
	system("cls");
	art.logoArt();
	art.directoryArt("Logout Confirmation");
	

	cout << "\x1B[33mAre you sure you want to logout?\033[0m\n";
	
	char option;
	cout << "Enter your response (Y/N) : ";
	cin >> option;

	switch (option){
	case 'Y':
		cout << "\x1B[32mYou now have been logged out\033[0m\n";
		exit(0);
	case 'N':
		return false;
	default:
		break;
	}

	return true;
}

//function to check if user exist in DB
boolean checkUserExist(string userIC) {

	//define sql statement and connection state
	string query = "SELECT COUNT(*) FROM users WHERE staffIC ='" + userIC + "'";
	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	//check query execution
	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		int userCount = atoi(row[0]);
		mysql_free_result(res);
		
		//check if user exist in database
		if (userCount != 0) {
			return true;
		}
	}
	return false;
}

//function to check if data string is numeric
boolean isNumeric(string userData) {

	for (char c : userData) {
		if (!isdigit(c)) {
			return false;
		}
	}
	return true;

}

//function to create ID
string createID(string lastID, string prefix) {
	
	//remove prefix, and turn it into int datatype
	string lastNumberS= lastID.substr(3);
	int number;
	istringstream(lastNumberS) >> number;
	
	//add value to 1
	number++;
	
	//combine prefix with number
	ostringstream currentNum;
	currentNum << prefix << setw(4) << setfill('0') << number;

	string userID = currentNum.str();

	return userID;
}

//function to change the role value into string
string getRoleString(int userRole) {

	switch (userRole) {
	case 1:
		return "Superadmin";
		break;
	case 2:
		return "Nurse";
		break;
	case 3:
		return "Care Taker";
		break;
	}
	return "Unassigned";
}

//function to edit user - staffIC
void editUserStaffIC(string* userData) {

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
		else if (!isNumeric(staffIC)) {
			cout << "\n\x1B[33mStaff's IC number are invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			staffIC.clear();
		}
		else if (checkUserExist(staffIC)){
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
		"WHERE staffID ='" +userData[0]+ "'";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);
	
	cout << "\n\x1B[32mStaff IC has been successfully updated\033[0m\n";
	system("pause");
	staffMngtView();
}

//function to edit user - staffName
void editUserStaffName(string* userData){

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
	staffMngtView();
}

//function to edit user - staffPhone
void editUserStaffPhone(string* userData) {

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
		else if (!isNumeric(staffPhone)) {
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
	staffMngtView();
}

//function to edit user - editUserStaffRole
void editUserStaffRole(string* userData) {

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
			staffMngtView();
		}
	} while (continueLoop);

	string query =
		"UPDATE users SET "
		"staffRole =" + to_string(staffRole) + " WHERE staffID ='" + userData[0] + "'";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	cout << "\n\x1B[32mStaff Role has been successfully updated\033[0m\n";
	system("pause");
	staffMngtView();
}

//function to edit user
void editUser(string* userData) {
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
		cout << "\nSelect Column you want to edit\n";
		cout << "1 - Staff IC\n";
		cout << "2 - Staff Name\n";
		cout << "3 - Staff Phone\n";
		cout << "4 - Role\n";
		cout << "5 - Previous page\n";
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
		editUserStaffIC(userData);
	case 2:
		editUserStaffName(userData);
	case 3:
		editUserStaffPhone(userData);
	case 4:
		editUserStaffRole(userData);
	case 5:
		staffMngtView();
	}
}

//function to remove user
void removeUser(string* userData){
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
				staffMngtView();
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
			staffMngtView();
			break;
		default:
			break;
		}
	} while (continueLoop);
}

//function to find user
void searchUser() {

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
			editUser(userData);
			break;
		case 2: 
			//remove user
			continueLoop = false;
			system("pause");
			removeUser(userData);
			break;
		case 3 :
			staffMngtView();
		}
	} while (continueLoop);
}

//staff management sub-module (View all user)
void staffMngtView() {

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
						row_data.push_back(getRoleString(roleValue));
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
			cout << "2 - Previous page\n";
			cout << "Enter your choice : ";
			

			if (cin >> option) {
				switch (option) {
				case 1:
					continueLoop = false;
					option = 0;
					searchUser();
					break;
				case 2:
					cout << "Redirecting you to previous page\n";
					continueLoop = false;
					option = 0;
					system("pause");
					staffMngt();
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

//staff management sub-module (Register new user)
void staffMngtReg() {
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
		else if (!isNumeric(userIC)) {
			cout << "\x1B[33mUser's IC number are invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			userIC.clear();
		}
		else {
			break;
		}
	} while (true);

	if (checkUserExist(userIC)) {
		cout << "\n\x1B[33mUser already exist, redirecting you back to Staff Management Module\033[0m\n";
		system("pause");
		staffMngt();
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
		else if(!isNumeric(userPhone)){
			cout << "\n\x1B[33mUser's phone number was invalid\033[0m\n";
			cout << "\x1B[33mPlease try again\033[0m\n";
			userPhone.clear();

		}else {
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
		case 1 :
			userRole = 1;
			continueLoop = false;
			break;
		case 2 : 
			userRole = 2;
			continueLoop = false;
			break;
		case 3 : 
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
	userID = createID(passedID, "STF");
	
	string insertQuery =
		"INSERT INTO users"
		"(staffID,staffIC,staffName,staffPass,staffPhone,staffRole,isPassChanged) "
		"VALUES "
		"('"+userID+ "', '" +userIC+ "', '" +userName+ "' , 'MyPassword@77' , '" +userPhone+ "' ,'" +to_string(userRole)+ "',0)";
	
	const char* queryC = insertQuery.c_str();
	conState = mysql_query(connection, queryC);

	//Check if the query has successfully execute
	if (!conState) {

		printf("\x1B[32mSuccessfully registered the user\033[0m\n");
		cout << "\nDirecting you to Staff Management Module (SMM)\n";
		system("pause");
		staffMngt();

	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

}

//staff management module
void staffMngt() {
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
		staffMngt();
	}
	else {
		switch (option) {
		case 1:
			cout << "\nRedirecting you to Staff Registration\n";
			system("pause");
			staffMngtReg();
			break;
		case 2:
			cout << "\nRedirecting you to Staff List View\n";
			system("pause");
			staffMngtView();
			break;
		case 3:
			cout << "\nRedirecting you back to Main Menu\n";
			system("pause");
			moduleSuperadmin();
			break;
		case 4:
			system("pause");
			if (!loggingOut()) {
				staffMngt();
			}
			break;			
		}
	}
} 

//TODO
//Superadmin Menu
void moduleSuperadmin() {
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
		moduleSuperadmin();
	}
	else {
		switch (option) {
		case 1:
			// MSMM module
			// TODO
			break;
		case 2:
			// RMM Module
			// TODO
			break;
		case 3:
			// SMM
			cout << "\nRedirecting you to Staff Management Module\n";
			system("pause");
			staffMngt();
			break;
		case 4:
			system("pause");
			if(!loggingOut()){
				moduleSuperadmin();
			}
			break;
		default:
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			moduleSuperadmin();
		}
	}
}

//TODO
//function to catergorised user role
void userRoleSep(string* userData) {
	
	int userRole = stoi(userData[5]);
	switch (userRole) {
		case 1:
			moduleSuperadmin();
			break;
			system("pause");
		case 2:
			//userRole = Nurse
			break;
		case 3:
			//userRole = Caretaker
			break;
		default:
			cout << "\x1B[93m\nYour role has not been assigned by admin yet, please contact admininstrator\033[0m\n";
			system("pause");
			exit(0);
			break;
	}
}

//function to check if the password stored is still default password
void havePassChanged(string* userData) {
	
	cout << "Checking user information";
	art.loadingArt();

	int userRole = stoi(userData[6]);
	if (userRole == 0) {

		//user havent change the default password
		cout << "\x1B[33mYou haven't change your password\033[0m\n\n";
		cout << "\x1B[94mPlease change you password to access the system\033[0m\n";
		char ch;
		string password;

		do {
			
			cout << "Enter your new password : ";
			while ((ch = _getch()) != 13) {
				if (ch == 8) {
					if (!password.empty()) {
						password.pop_back();
						std::cout << "\b \b";
					}
				}
				else {
					password += ch;
					std::cout << '*';
				}
			}

			//password validation
			if (password == "Password@77") {
				cout << "\x1B[93m\nYour password can't be same as default password\033[0m\n";
				cout << "\x1B[93mPlease try again\033[0m\n";
				password.clear();
			}
			else if (password.length() <= 13) {
				cout << "\x1B[93m\nYour password is too short\033[0m\n";
				cout << "\x1B[93mPlease try again\033[0m\n";
				password.clear();
			}
			else if (!any_of(password.begin(), password.end(), ::isupper)) {
				cout << "\x1B[93m\nYour password must contain atleast one uppercase letter\033[0m\n";
				cout << "\x1B[93mPlease try again\033[0m\n";
				password.clear();
			}
			else if (!any_of(password.begin(), password.end(), ::islower)) {
				cout << "\x1B[93m\nYour password must contain atleast one lowercase letter\033[0m\n";
				cout << "\x1B[93mPlease try again\033[0m\n";
				password.clear();
			}
			else if (!any_of(password.begin(), password.end(), ::isdigit)) {
				cout << "\x1B[93m\nYour password must contain at least one digit\033[0m\n";
				cout << "\x1B[93mPlease try again\033[0m\n";
				password.clear();
			}
			else if (password.find_first_of("!@#$%^&*()-_=+[]{}|;:'\",.<>/?") == string::npos) {
				cout << "\x1B[93m\nYour password must contain at least one special character\033[0m\n";
				cout << "\x1B[93mPlease try again\033[0m\n";
				password.clear();
			}
			else {
				break;
			}

		} while (true);
			
		//update password into db
		string query =
			"UPDATE users "
			"SET staffPass ='" +password+ "' , isPassChanged = 1 "
			"WHERE staffID ='" +userData[0]+"'";

		const char* queryC = query.c_str();
		if (mysql_query(connection, queryC) == 0) {
			cout << "\x1B[32m\nYou have successfully changed the password\033[0m\n" << endl;
			userRoleSep(userData);
		}
		else {
			cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	}
	else {

		//user have change the default password
		cout << "\x1B[32mUser information validated\033[0m\n\n";
		system("pause");
		userRoleSep(userData);
	}
}

//function to handle login interaction
void login() {

	system("cls");
	art.logoArt();
	string staffIC, password;

	cout << "\x1B[94mPlease login to access the system\033[0m\n";
	cout << "Enter your IC number : ";
	cin >> staffIC;

	string query = "SELECT staffID FROM users WHERE staffIC = '" + staffIC + "'";
	const char* queryC = query.c_str();
	conState = mysql_query(connection, queryC);

	//check query execution
	if (!conState) {
		res = mysql_store_result(connection);
		if (res->row_count == 1) {

			//found user
			mysql_free_result(res);
			printf("\x1B[32mUser found\033[0m\n\n");
			cout << "Enter your password : ";
			char ch;
			while ((ch = _getch()) != 13) {
				if (ch == 8) {
					if (!password.empty()) {
						password.pop_back();
						std::cout << "\b \b";
					}
				}
				else {
					password += ch;
					std::cout << '*';
				}
			}

			string query = "SELECT * FROM users WHERE staffIC = '" + staffIC + "' and staffPass ='" +password+ "'";
			const char* queryC = query.c_str();
			conState = mysql_query(connection, queryC);

			if (!conState) {
				res = mysql_store_result(connection);
				if (res->row_count == 1) {

					printf("\x1B[32m\nYou've successfully login\033[0m\n");
					
					row = mysql_fetch_row(res);

					int loginRoleint = stoi(row[5]);
					string loginRole;
					switch (loginRoleint) {
						case 1:
							loginRole = "superadmin";
							break;
						case 2:
							loginRole = "Nurse";
							break;
						case 3:
							loginRole = "Caretaker";
							break;
					}
					
					//coverting mysql_row into array
					string userData[7];

					for (int i = 0; i < 7; i++) {
						if (row[i] != nullptr) {
							userData[i] = row[i];
						}
						else {
							userData[i] = "NULL";
						}
					}
					
					cout << "Logging in as " << loginRole << endl << endl;
					havePassChanged(userData);
				}
				else {
					cout << "\x1B[31m\nYou've entered the wrong password. Please try again\033[0m\n" << endl;
					system("pause");
					login();
				}
			}
			else {
				cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}

		}
		else {
			// user not found
			printf("\x1B[31mCannot find such user\033[0m\n\n");
			cout << "Please try again\n";
			system("pause");
			mysql_free_result(res);
			login();
		}
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

int main() {

	//debug mode
	int mode;
	cout << "1 - Normal\n";
	cout << "2 - Debug\n";
	cout << "Please select mode : ";
	cin >> mode;

	if (mode == 1) {
		system("cls");
		//NORMAL MODE
		
		//check if the connection unable to establish
		if (!connectionFunction()) {
			cout << "Program error, please try again\n\n";
			exit(0);
		}
		if (!firstTimeSetup()) {
			cout << "Program error, please try again\n\n";
			exit(0);
		}
		system("pause");
		login();
		return 0;
	}
	else if (mode == 2) {
		//debug mode
		system("cls");
		if (connectionFunction() == false) {
			cout << "Program error, please try again\n\n";
			exit(0);
		}
		firstTimeSetup();
		staffMngt();
	}
}