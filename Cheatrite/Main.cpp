#include <iostream>
#include <string>
#include <Windows.h>
#include "Cheatrite.h"

using namespace std;

int main() {
	// Fixed the CodeMaid formatting lol
	string selectedchamp;
pickChamp:
	{
		cout << "What champion would you like to play?" << endl;
		cout << "Available champions: Ashka, Jade, Pearl, Ezmo, Raigon" << endl;
		cout << "Note: Champion names are case sensitive (for now)." << endl;
		cin >> selectedchamp;
	}

	if (selectedchamp == "Ashka" || selectedchamp == "Jade" || selectedchamp == "Pearl" || selectedchamp == "Ezmo" || selectedchamp == "Raigon") {
		Cheatrite Cheatrite(selectedchamp);
		Cheatrite.run();
	}
	else {
		cout << endl << endl << "Wrong champion type." << endl; // formatting ftw
		goto pickChamp;
	}
	return EXIT_SUCCESS;
}