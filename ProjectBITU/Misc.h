#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <string>
#include <mysql.h>
#include <sstream>
#include <iomanip>

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

};
#endif // !MISC_H

