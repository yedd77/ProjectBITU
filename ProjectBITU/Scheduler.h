#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <mysql.h>
#include <iostream>
#include <vector>
#include "Misc.h"

using namespace std;

extern int conState;
extern MYSQL* connection;
extern MYSQL_ROW row;
extern MYSQL_RES* res;
extern Misc misc;


class Scheduler {
public :

	//function to check if today schedule count is same as prescription count
	void checkSchedule() {

		//define query 
		string checkTodaySchedule = R"(
			SELECT
                CASE
                    WHEN (
                        SELECT SUM(timePerDay) 
                        FROM prescriptions
                    ) = (
                    SELECT COUNT(*) 
                    FROM schedules
                    WHERE date = CURDATE()
               ) THEN 1
               ELSE 0
           END AS result;
		)";
        const char* cts = checkTodaySchedule.c_str();
        conState = mysql_query(connection, cts);

        if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);;
			mysql_free_result(res);

			//Chech if today  schedule count is same as prescription count
            if (stoi(row[0]) == 0) {
				getPrescriptID();
            }
		}
	}

    //function to get prescription ID
	void getPrescriptID() {

		string getUnscheduled = "SELECT DISTINCT p.prescriptID "
			"FROM prescriptions p "
			"LEFT JOIN schedules s  "
			"ON p.prescriptID = s.prescriptID  "
			"AND s.date = CURDATE() "
			"WHERE s.prescriptID IS NULL ";
		const char* gu = getUnscheduled.c_str();
		conState = mysql_query(connection, gu);

		if (!conState) {
			res = mysql_store_result(connection);
			vector<string> prescriptID;
			while (row = mysql_fetch_row(res)) {
				prescriptID.push_back(row[0] ? row[0] : "NULL");
			}

			mysql_free_result(res);
			generateSchedule(prescriptID);
		}
		else {
			cout << "Error: " << mysql_error(connection) << endl;
		}
	}

	//function to generate schedule
	void generateSchedule(vector <string> prescriptID) {

		//each prescription may have multiple schedule, therefore we need to get
		//how many schedule for each prescription and until the program got the data
		// it will generate row for each schedule

		//loop through each prescription ID
		for (const auto& id : prescriptID) {
			
			vector <string> timeInterval;
			int frequency = 0;
			//define query to get how many schedule need to be generated
			string getScheduleCount = "SELECT timePerDay "
				"FROM prescriptions WHERE prescriptID = '" + id + "'";
			const char* gsc = getScheduleCount.c_str();
			conState = mysql_query(connection, gsc);
			
			if (!conState) {
				res = mysql_store_result(connection);
				row = mysql_fetch_row(res);
				timeInterval = frequencyScheduler(atoi(row[0]));
			}
			else {
				cout << "Error: " << mysql_error(connection) << endl;
			}

			string scheduleID;
			boolean isGiven = false;

			//loop through each time interval
			for (const auto& time : timeInterval) {

				scheduleID.clear();
				 
				//schedule ID instance
				scheduleID = getLastID();

				//define query to insert schedule
				string insertSchedule = "INSERT INTO schedules (scheduleID, prescriptID, timeGiven, date, isGiven) "
					"VALUES ('" +scheduleID+"','" + id + "','" + time +"' , CURDATE() , " +to_string(isGiven)+  ")";
				const char* is = insertSchedule.c_str();
				conState = mysql_query(connection, is);
				
	
				if (conState) {
					cout << "Error: " << mysql_error(connection) << endl;
				}
			}
		}
		cout << "\n\x1B[32mSchedule has been updated successfully\033[0m\n";
	}
	
	//this function take frequency as parameter and return
	//the time when the resident need to take the medicine
	vector <string> frequencyScheduler(int frequency) {

		vector <string> time;

		switch (frequency) {
		case 1:
			time.push_back("0800");
			break;
		case 2:
			time.push_back("0800");
			time.push_back("2000");
			break;
		case 3:
			time.push_back("0600");
			time.push_back("1400");
			time.push_back("2200");
			break;
		case 4:
			time.push_back("0600");
			time.push_back("1200");
			time.push_back("1800");
			time.push_back("0000");
			break;
		}

		return time;
	}

	//function to get last ID on the schedule table
	string getLastID() {

		string getLastID = "SELECT scheduleID FROM schedules ORDER BY scheduleID DESC LIMIT 1";
		const char* gli = getLastID.c_str();
		conState = mysql_query(connection, gli);

		if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);
			string lastID = (row) ? row[0] : "000000000";
			string scehduleID = misc.createScheduleID(lastID, "SCD");
			mysql_free_result(res);

			return scehduleID;
		}
		else {
			cout << "Error: " << mysql_error(connection) << endl;
		}
		return "NULL";
	}
};
#endif // !SCHEDULER_H
