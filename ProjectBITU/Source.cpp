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
#include "Auth.h"
#include "SuperAdmin.h"
#include "Misc.h"
#include "Scheduler.h"
#include "dailyUpdater.h"

//debug
#include "MedStrgModule.h"

//debug
MedStrgModule medStrgModule;

using namespace std;

int conState;
MYSQL* connection;
MYSQL_ROW row;
MYSQL_RES* res;

Artwork art;
SuperAdmin admin;
Auth auth;
Misc misc;
Scheduler scheduler;
DailyUpdater dailyUpdater;


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

int main() {
	system("cls");
	//NORMAL MODE

	//check if the connection unable to establish
	if (!connectionFunction()) {
		cout << "Program error, please try again\n\n";
		exit(0);
	}
	if (!auth.firstTimeSetup()) {
		cout << "Program error, please try again\n\n";
		exit(0);
	}	
	scheduler.checkSchedule();
	dailyUpdater.controller();
	system("pause");
	auth.login();
	
	return 0;
}