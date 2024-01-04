#ifndef VERIFIER_H
#define VERIFIER_H

#include <string>
#include "Misc.h"

extern	Misc misc;
using namespace std;

class Verifier {

public :

	//this function is used to verify the user authorization of a page
	//is used when a user try to access a page that is need superadmin level of access
	bool verifyUserAuthorization(string userID) {
		
		//define the query
		string getRoleQuery = "SELECT staffRole FROM users WHERE staffID = '" + userID + "'";
		const char* grq = getRoleQuery.c_str();
		conState = mysql_query(connection, grq);

		if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			int role = atoi(row[0]);
			mysql_free_result(res);

			//check if the user is superadmin
			if (role == 1) {
				return true; //user is superadmin
			}
			else {
				return false; //user is not superadmin
			}
		}
		else{
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			return false;
		}
	}
};
#endif // !VERIFIER_H
