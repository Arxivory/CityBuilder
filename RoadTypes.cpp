#include "RoadTypes.h"

string roadTypeToString(RoadType type) {
    switch (type) {
    case RoadType::STRAIGHT: return "Straight";
    case RoadType::TURN: return "Turn";
    case RoadType::INTERSECTION: return "Intersection";
    default: return "Unknown";
    }
}