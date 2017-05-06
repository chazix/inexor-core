#pragma once
#include "inexor/fpsgame/game.hpp"
#include "inexor/fpsgame/network_types.hpp"

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
