#include "Records.hpp"
#include <stdlib.h>
Records::Records(size_t sz)
{
	maxSize = sz;
	records = (PlayerInformation*)malloc(sz * sizeof(PlayerInformation));
}

Records::~Records()
{
	free(records);
}

PlayerInformation* Records::add(float x, float y)
{
	records[count%maxSize].x = x;
	records[count%maxSize].y = y;
	count += 1;
	return getLatest();
}

PlayerInformation* Records::get(size_t index)
{
	return &(records[index%maxSize]);
}

PlayerInformation* Records::getLatest()
{
	return get(count);
}

PlayerInformation* Records::getOldest()
{
	size_t index = count - maxSize - 1;
	if (index < 0)
		index = 0;

	return get(index);
}
