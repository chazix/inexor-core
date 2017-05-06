#pragma once

struct clientmode
{
    virtual ~clientmode() {}

    virtual void preload() {}
    virtual int clipconsole(int w, int h) { return 0; }
    virtual void drawhud(fpsent *d, int w, int h) {}
    virtual bool isinvisible(fpsent *d) { return false; }
    virtual void rendergame() {}
    virtual void respawned(fpsent *d) {}
    virtual void setup() {}
    virtual void checkitems(fpsent *d) {}
    virtual int respawnwait(fpsent *d) { return 0; }
    virtual void pickspawn(fpsent *d) { findplayerspawn(d); }
    virtual void senditems(packetbuf &p) {}
    virtual void removeplayer(fpsent *d) {}
    virtual void gameover() {}
    virtual bool hidefrags() { return false; }
    virtual int getteamscore(const char *team) { return 0; }
    virtual void getteamscores(vector<teamscore> &scores) {}
    virtual void aifind(fpsent *d, ai::aistate &b, vector<ai::interest> &interests) {}
    virtual bool aicheck(fpsent *d, ai::aistate &b) { return false; }
    virtual bool aidefend(fpsent *d, ai::aistate &b) { return false; }
    virtual bool aipursue(fpsent *d, ai::aistate &b) { return false; }
    virtual void killed(fpsent *d, fpsent *actor) {}
    virtual void gameconnect(fpsent *d) {}
    virtual void renderscoreboard(g3d_gui &g, scoregroup &sg, int fgcolor, int bgcolor) {}
};

