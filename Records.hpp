#pragma once
#include "PlayerInformation.hpp"

// Holds a fixed number of records of type PlayerInformation*
class Records
{
private:
	PlayerInformation* records;
	size_t count = 0;
	size_t maxSize = 0;
public:
	PlayerInformation* add(float x, float y);				// adds a new record
	PlayerInformation* get(size_t index);		// get record at provided index
	PlayerInformation* getLatest();
	PlayerInformation* getOldest();
	Records(size_t sz);							// must initialise class with a specific size
	~Records();
};