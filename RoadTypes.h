#pragma once
#ifndef ROADTYPES_H
#define ROADTYPES_H
#include <string>

using namespace std;

enum class RoadType {
	STRAIGHT,
	TURN,
	INTERSECTION
};

string roadTypeToString(RoadType type);

#endif
