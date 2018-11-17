#include <ctime>

struct PlayerInformation
{
	float x;
	float y;
	float previousX;
	float previousY;
	float velocityX;
	float velocityY;
	clock_t lastUpdate;
	bool invulnerable;
	bool casting;
	bool castingImportant;
};