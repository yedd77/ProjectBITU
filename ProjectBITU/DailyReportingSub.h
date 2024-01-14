#ifndef DAILYREPORTINGSUB_H
#define DAILYREPORTINGSUB_H

#include <string>
using namespace std;

class DailyReportingSub {

public:
	void dailyReportMenu();
	void displayPrescriptionReport();
	void batchReportMenu();
	void DisplayCurrentStock();
	void displayExpiredBatch();
	void lowStockBatch();
	void displayzeroStockBatch();

};
#endif // !DAILYREPORTINGSUB_H
