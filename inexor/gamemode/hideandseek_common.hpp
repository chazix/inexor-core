#pragma once

#define TEAM_HIDE "hide"
#define TEAM_SEEK "seek"

#define ishider(ci) (strcmp(ci->team, TEAM_HIDE) == 0 && ci->state.state != CS_SPECTATOR ? true : false)
#define isseeker(ci) (strcmp(ci->team, TEAM_SEEK) == 0 && ci->state.state != CS_SPECTATOR ? true : false)

struct hideandseekmode
{

};
