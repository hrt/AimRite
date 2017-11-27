#include <iostream>
#include <string>
#include <Windows.h>
#include "Cheatrite.h"

using namespace std;

int main() {
	string selectedchamp;
	while (!(selectedchamp == "Ashka" || selectedchamp == "Jade" || selectedchamp == "Pearl" || selectedchamp == "Ezmo" || selectedchamp == "Raigon"))
	{
		cout << "What champion would you like to play?" << endl;
		cout << "Available champions: Ashka, Jade, Pearl, Ezmo, Raigon" << endl;
		cout << "Note: Champion names are case sensitive (for now)." << endl;
		cin >> selectedchamp;
	}

	Cheatrite Cheatrite(selectedchamp);
	Cheatrite.run();

	return EXIT_SUCCESS;
}