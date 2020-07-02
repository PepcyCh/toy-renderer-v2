#include "GIDManager.h"

namespace pepcy::gid {

GIDManager gid_mgr;

GIDManager::GIDManager() {
    last.id = 0;
}

GID GIDManager::NewGID() {
    if (freed.empty()) {
        return NextGID();
    } else {
        GID ret = freed.back();
        freed.pop_back();
        return ret;
    }
}

void GIDManager::FreeGID(const GID &id) {
    freed.emplace_back(id);
}

GID GIDManager::NextGID() {
    GID ret = last;
    ++last.id;
    return ret;
}

}