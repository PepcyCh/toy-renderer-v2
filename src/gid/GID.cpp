#include "GID.h"
#include "GIDManager.h"

#include <iostream>

namespace pepcy::gid {

bool GID::operator==(const GID &rhs) const {
    return id == rhs.id;
}

GID NewGID() {
    return gid_mgr.NewGID();
}

void FreeGID(const GID &id) {
    gid_mgr.FreeGID(id);
}

}