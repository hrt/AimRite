#include <iostream>
#include <string>
#include <Windows.h>
#include "Cheatrite.h"

using namespace std;

int main() {
	string selectedChampion;
	while (!(selectedChampion == "Ashka"
		|| selectedChampion == "Jade"
		|| selectedChampion == "Pearl"
		|| selectedChampion == "Ezmo"
		|| selectedChampion == "Raigon"
		|| selectedChampion == "Poloma"))
	{
		cout << "What champion would you like to play?" << endl;
		cout << "Available champions: Ashka, Jade, Pearl, Ezmo, Raigon" << endl;
		cout << "Note: Champion names are case sensitive (for now)." << endl;
		cin >> selectedChampion;
	}

	Cheatrite Cheatrite(selectedChampion);
	Cheatrite.run();

	return EXIT_SUCCESS;
}