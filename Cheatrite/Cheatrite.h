#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <tlhelp32.h>

#include <Core\MemoryManager.h>
#include <Core\MouseManager.hpp>
#include <Core\Offsets.hpp>

using namespace std;

// don't worry about this, I'm trying to make everything more easily accessible
// that is why this is here, but I just stopped at this point
// gonna work more on it later
enum Champions
{
	JADE,
};

class Cheatrite
{
public:
	Cheatrite(string champ);
	~Cheatrite();

	void run();
private:
	string champion;
};
