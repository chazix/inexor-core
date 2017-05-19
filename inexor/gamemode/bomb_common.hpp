#pragma once
#include "inexor/shared/geom.hpp"

namespace server
{

struct spawnloc
{
    vec o;
    int team, index;
    int cn = -1;
    spawnloc() = default;
    spawnloc(const vec& o_, int team_, int index_) : o(o_), team(team_), index(index_), cn(-1) {}
};

struct bombmode
{

};
} // ns server
