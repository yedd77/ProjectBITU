#include "Auth.h"
#include <iostream>
#include <string>
#include <mysql.h>
#include "Artwork.h"
#include <conio.h>
#include <algorithm>
#include "SuperAdmin.h"
#include "config.h"
#include "MedStrgModule.h"
#include "ResidentMgmntModule.h"
#include "Verifier.h"

using namespace std;

extern int conState;
extern MYSQL* connection;
extern MYSQL_ROW row;
extern MYSQL_RES* res;
extern Artwork art;
extern SuperAdmin admin;
MedStrgModule medStrg;
ResidentMgmntModule resMgmnt;
config conf;
Verifier verifier;

//function to initialize the system if the system hasnt been initialize before
bool Auth::firstTimeSetup() {

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
			string query = "INSERT INTO users(staffID,staffIC,staffName,staffPass,staffPhone,staffRole,isPassChanged) VALUES ('STF0001' , '000000000000' , 'Super Admin' , 'MyPassword@777' , '000000000000' , 1 , 0 )";
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

//function to handle login interaction
void  Auth::login() {

	system("cls");
	art.logoArt();
	string staffIC, password;

	cout << "\x1B[96mPlease login to access the system\033[0m\n";
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

			string query = "SELECT * FROM users WHERE staffIC = '" + staffIC + "' and staffPass ='" + password + "'";
			const char* queryC = query.c_str();
			conState = mysql_query(connection, queryC);

			if (!conState) {
				res = mysql_store_result(connection);
				if (res->row_count == 1) {

					printf("\x1B[32m\nYou've successfully login\033[0m\n");

					row = mysql_fetch_row(res);
					conf.setCurrentUserID(row[0]);
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
					hasPassChanged(userData);
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

//function to check if the password stored is still default password
void  Auth::hasPassChanged(string* userData) {

	cout << "Checking user information";
	art.loadingArt();

	int userRole = stoi(userData[6]);
	if (userRole == 0) {

		//user havent change the default password
		cout << "\x1B[33mYou haven't change your password\033[0m\n\n";
		cout << "\x1B[96mPlease change you password to access the system\033[0m\n";
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
			"SET staffPass ='" + password + "' , isPassChanged = 1 "
			"WHERE staffID ='" + userData[0] + "'";

		const char* queryC = query.c_str();
		if (mysql_query(connection, queryC) == 0) {
			cout << "\x1B[32m\nYou have successfully changed the password\033[0m\n" << endl;
			seperateUserMenu(userData);
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
		seperateUserMenu(userData);
	}
}

void Auth::seperateUserMenu(string* userData) {

	int userRole = stoi(userData[5]);
	switch (userRole) {
	case 1:
		admin.superAdminMenu();
		system("pause");
		break;
	case 2:
		medStrg.nurseModuleMenu();
		system("pause");
		break;
	case 3:
		resMgmnt.residentMenu();
		system("pause");
		break;
	default:
		cout << "\x1B[93m\nYour role has not been assigned by admin yet, please contact admininstrator\033[0m\n";
		system("pause");
		exit(0);
		break;
	}
}

bool Auth::logout() {

	system("cls");
	art.logoArt();
	art.directoryArt("Logout Confirmation");


	cout << "\x1B[33mAre you sure you want to logout?\033[0m\n";

	char option;
	cout << "Enter your response (Y/N) : ";
	cin >> option;

	switch (option) {
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
