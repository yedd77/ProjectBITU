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

	checkWillExpire();
	checkHasLowStock();

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

//this function going to select all the batch where their date of expiry almost passed
//and remove it from database
void DailyUpdater::checkWillExpire(){

	do {
		//check if there is a batch that will passed their date of expiry in less than 20 days
		string checkWillExpire = "SELECT "
			"COUNT(*) AS batchCount "
			"FROM "
			"batches B "
			"JOIN "
			"medicines M ON B.medsID = M.medsID "
			"WHERE "
			"B.batchExpiry < CURRENT_DATE + INTERVAL 20 DAY ";
		const char* cwe = checkWillExpire.c_str();
		conState = mysql_query(connection, cwe);

		if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);

			if (atoi(row[0]) < 0) {
				message.push_back("No medication batches with past date expiry in 20 days from now");
				break;
			}
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}

		//define query where their date of expiry have will passed in less than 20 days
		string selectBatch = "SELECT B.batchID, B.medsID, M.medsName, B.batchQuantity, "
			"DATEDIFF(B.batchExpiry, CURRENT_DATE) AS daysUntilExpiry "
			"FROM batches B "
			"JOIN medicines M ON B.medsID = M.medsID "
			"WHERE DATEDIFF(B.batchExpiry, CURRENT_DATE) >= 0 AND DATEDIFF(B.batchExpiry, CURRENT_DATE) <= 20 ";
		const char* sb = selectBatch.c_str();
		conState = mysql_query(connection, sb);

		if (!conState) {
			res = mysql_store_result(connection);
			while (row = mysql_fetch_row(res)) {
				message.push_back(string(row[0]) + " -  " + string(row[2]) + " with medsID of " + string(row[1]) + " will expire in " + string(row[4]) + " day(s)");
			}

			break;
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	}while(true);
}

//this function going to check if there's any batch that low in stocks less than 30
void DailyUpdater::checkHasLowStock(){

	do {
		//check if theres any batch that have low stock
		string countBatchLow = "SELECT COUNT(B.batchID) AS NumberOfBatches "
			"FROM Batches B "
			"JOIN Medicines M ON B.medsID = M.medsID "
			"WHERE B.batchQuantity <= 30 ";
		const char* cbl = countBatchLow.c_str();
		conState = mysql_query(connection, cbl);

		if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);

			if (atoi(row[0]) < 0) {
				message.push_back("No medication batches with low stock");
				break;
			}
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			cout << "4" << endl;
			exit(0);
		}

		//select all the batch that have low stock less than 30
		string selectBatchLow = "SELECT B.batchID, B.medsID, M.medsName, B.batchQuantity "
			"FROM Batches B "
			"JOIN Medicines M ON B.medsID = M.medsID "
			"WHERE B.batchQuantity <= 30 ";
		const char* sbl = selectBatchLow.c_str();
		conState = mysql_query(connection, sbl);
		
		if (!conState) {

			res = mysql_store_result(connection);
			while (row = mysql_fetch_row(res)) {
				message.push_back(string(row[0]) + " -  " + string(row[2]) + " with medsID of " + string(row[1]) + " has currently low in stock [ " + string(row[3])+ " ]");
			}
			break;
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			cout << "5" << endl;
			exit(0);
		}
	} while (true);
}



