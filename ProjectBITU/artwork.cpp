#include <iostream>
#include <string>
#include <iomanip>
#include <windows.h>
#include "Artwork.h"

using namespace std;

//logo used in every page of the program
void Artwork::logoArt() {

	string line = "\x1B[97m-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-#-\033[0m\n";

	cout << line;
	std::cout << "\033[97m";
	cout << R"(
                                             ________  ______  ___ _____ 
                                            |  ___|  \/  ||  \/  |/  ___|
                                            | |__ | .  . || .  . |\ `--. 
                                            |  __|| |\/| || |\/| | `--. \
                                            | |___| |  | || |  | |/\__/ /
                                            \____/\_|  |_/\_|  |_/\____/ 

                                         Elderly Medication Management System    
          
)";
	std::cout << "\033[0m";
	cout << line << endl;
}

//directory art used in every page of the program
void Artwork::directoryArt(string text) {

	/* create a buffer info which control all information abt console and
	* retrive the it. From the bufferInfo, the program retrieve rightmost and
	* leftmost column of the windows to calculate the width of the terminal
	* window. To find the right padding, the terminal width is subtracted with text length
	* gotten from the parameter and divided by 2. This function is to center the text in the
	* terminal*/

	CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bufferInfo);
	int terminalWidth = bufferInfo.srWindow.Right - bufferInfo.srWindow.Left + 1;

	size_t textWidth = text.length();
	size_t rightPadding = (terminalWidth - textWidth) / 2;

	cout << "------------------------------------------------------------------------------------------------------------------------\n";
	cout << setw(rightPadding + textWidth) << text << endl;
	cout << "------------------------------------------------------------------------------------------------------------------------\n";
}

//loading animation 
void Artwork::loadingArt() {

	Sleep(1000);
	cout << " .";
	Sleep(1000);
	cout << " .";
	Sleep(1000);
	cout << " .\n";
	Sleep(2000);
}