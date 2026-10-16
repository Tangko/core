#pragma once
#include <ctime>
#include <string>

struct ArrowRecord {
	bool active = false;
	uint32 team = 0;
	uint32 mapId = 0;
	float x = 0, y = 0, z = 0, o = 0;
	std::string casterName;
	std::string mapName;
	time_t expireTime = 0;
};

extern ArrowRecord g_arrowRecords[2];