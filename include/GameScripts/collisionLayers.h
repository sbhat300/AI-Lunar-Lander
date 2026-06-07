#ifndef COLLISIONLAYERS_H
#define COLLISIONLAYERS_H

#include <cstdint>

enum collisionLayer : std::uint32_t {
    NONE = 0,
    LANDER = 1 << 0
};

#endif