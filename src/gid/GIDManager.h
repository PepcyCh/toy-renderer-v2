#pragma once

#include <vector>

#include "GID.h"

namespace pepcy::gid {

class GIDManager {
  public:
    GIDManager();

    GID NewGID();
    void FreeGID(const GID &id);

  private:
    GID NextGID();

    GID last;
    std::vector<GID> freed;
};

extern GIDManager gid_mgr;

}