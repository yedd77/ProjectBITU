#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <string>
#include <mysql.h>
#include <sstream>
#include <iomanip>
#include <regex>
#include <vector>
#include <ctime>

using namespace std;

extern int conState;
extern MYSQL* connection;
extern MYSQL_ROW row;
extern MYSQL_RES* res;

class Misc {

public:

	//function to check if data string is numeric
	bool isNumeric(string userData) {

		for (char c : userData) {
			if (!isdigit(c)) {
				return false;
			}
		}
		return true;
	}

	bool checkUserExist(string userIC) {

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

	//function to check if meds name exist in database
	bool checkMedsNameExist(string medsName) {

		//define sql statement and connection state
		string query = "SELECT COUNT(*) FROM medicines WHERE medsName ='" + medsName + "'";
		const char* cu = query.c_str();
		conState = mysql_query(connection, cu);

		//check query execution
		if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			int medsCount = atoi(row[0]);
			mysql_free_result(res);

			//check if meds exist in database
			if (medsCount != 0) {
				return true;
			}
		}
		return false;
	}

	//function to create ID
	string createID(string lastID, string prefix) {

		//remove prefix, and turn it into int datatype
		string lastNumberS = lastID.substr(3);
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

	//function to validate date format
	bool isValidDateFormat(string &date) {

		//2029-12-01
		//checf if the date format is correct
		if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
			cout << "\x1B[33mInvalid format of date, please try again\033[0m\n";
			return false;
		}

		//check if year is numeric
		if (!isNumeric(date.substr(0, 4))) {
			cout << "\x1B[33mInvalid format of year, please try again\033[0m\n";
			return false;
		}

		//check if year is valid
		int year = stoi(date.substr(0, 4));
		if (year < 2023 && year > 2300) {
			cout << "\x1B[33mInvalid expiry year, please try again\033[0m\n";
			return false;
		}

		//check if month is numeric
		int month = stoi(date.substr(5, 2));
		if (!isNumeric(date.substr(5,2))) {
			cout << "\x1B[33mInvalid format of month, please try again\033[0m\n";
			return false;
		}

		//check if month is valid
		if (month > 12 && month < 1) {
			cout << "\x1B[33mInvalid expiry month, please try again\033[0m\n";
			return false;
		}

		//check if day is numeric
		if (!isNumeric(date.substr(8, 2))) {
			cout << "\x1B[33mInvalid format of day, please try again\033[0m\n";
			return false;
		}

		//check if day is valid
		int day = stoi(date.substr(8, 2));
		if (day >= 31 && day <= 1) {
			cout << "\x1B[33mInvalid expiry date, please try again\033[0m\n";
			return false;
		}
		return true;
	}
};
#endif // !MISC_H

