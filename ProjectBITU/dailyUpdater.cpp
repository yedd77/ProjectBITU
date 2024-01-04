#include "dailyUpdater.h"
#include <string>
#include <vector>
#include "Misc.h"
#include "table.h"
#include "Artwork.h"

extern Misc misc;
extern Artwork art;

using namespace std;

vector <string> message;

//this function is called in main()
//handling the daily update of multiple table
void DailyUpdater::controller(){

	system("pause");
	system("cls");
	art.logoArt();

	//call function to update schedule
	ScheduleUpdater();
	//call function to update batch
	batchUpdater();

	//display all message
	clitable::Table messageTable;
	clitable::Column rc[2] = {
		clitable::Column("Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Message",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 105, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 2; i++) { messageTable.addColumn(rc[i]); }

	string messageData[2];
	int count = 0;
	for (const auto& i : message) {
		messageData[0] = to_string(++count);
		messageData[1] = i;
		messageTable.addRow(messageData);
	}
	

	cout << messageTable.draw() << endl;

}

//this function going to select all the schedule where their date have passed
//and remove it from database
void DailyUpdater::ScheduleUpdater(){

	//define query
	string selectSchedule = "SELECT * FROM schedules WHERE date < CURDATE()";
	const char* ss = selectSchedule.c_str();
	conState = mysql_query(connection, ss);

	if (!conState) {
		res = mysql_store_result(connection);

		if (mysql_num_rows(res) > 0) {

			int fieldCount = 0;
			while (row = mysql_fetch_row(res)) {

				//define query for deletion
				string deleteSchedule = "DELETE FROM schedules WHERE scheduleID = '" + string(row[0])+ "' ";
				const char* ds = deleteSchedule.c_str();
				conState = mysql_query(connection, ds);

				if (conState) {
					cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
					exit(0);
				}

				fieldCount++;
			}
			message.push_back(fieldCount + " medication(s) schedule has been adjusted");
		}
		else {
			message.push_back("No update for medication schedule");
		}
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection)  << endl;
		exit(0);
	}
}

//this function going to select all the batch where their date of expiry have passed
//and remove it from database
void DailyUpdater::batchUpdater(){

	//define query where their date of expiry have passed
	string selectBatch = "SELECT B.batchID, B.medsID, M.medsName, B.batchQuantity "
		"FROM batches B "
		"JOIN medicines M ON B.medsID = M.medsID "
		"WHERE B.batchExpiry < CURRENT_DATE ";
	const char* sb = selectBatch.c_str();
	conState = mysql_query(connection, sb);

	if (!conState) {
		res = mysql_store_result(connection);

		if (mysql_num_rows(res) > 0) {

			while (row = mysql_fetch_row(res)) {

				message.push_back(string(row[0]) + " - " + string(row[3]) + " " + string(row[2]) + " with medsID of " + string(row[1]) + " has expired and removed");

				//define query for deletion
				string deleteBatch = "DELETE FROM batches WHERE batchID = '" + string(row[0])+ "' ";
				const char* db = deleteBatch.c_str();
				conState = mysql_query(connection, db);

				if (conState) {
					cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
					exit(0);
				}
			}

		}
		else {
			message.push_back("No medication batches with past date expiry");
		}
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	//define query where stock is 0
	string selectBatchQty = "SELECT B.batchID, B.medsID, M.medsName, B.batchQuantity "
		"FROM Batches B "
		"JOIN Medicines M ON B.medsID = M.medsID "
		"WHERE B.batchQuantity <= 0 ";
	const char* sbq = selectBatchQty.c_str();
	conState = mysql_query(connection, sbq);

	if (!conState) {
		res = mysql_store_result(connection);

		if (mysql_num_rows(res) > 0) {

			while (row = mysql_fetch_row(res)) {

				message.push_back(string(row[0]) + " - " + string(row[3]) + " " + string(row[2]) + " with medsID of " + string(row[1]) + " didn't have stocks and removed");

				//define query for deletion
				string deleteBatchqty = "DELETE FROM batches WHERE batchID = '" + string(row[0])+ "' ";
				const char* dbq = deleteBatchqty.c_str();
				conState = mysql_query(connection, dbq);

				if(conState){
					cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
					exit(0);
				}
			}
		}
		else {
			message.push_back("No medication batches that zero in stocks");
		}
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		cout << 6 << endl;
		exit(0);
	}
}