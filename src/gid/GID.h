#pragma once

#include <cstddef>
#include <iostream>

namespace pepcy::gid {

class GID {
  public:
    bool operator==(const GID &rhs) const;

  private:
    unsigned long long id;

    friend struct GIDHasher;
    friend class GIDManager;

    friend std::ostream &operator<<(std::ostream &out, const GID &gid) {
        return out << "GID[" << gid.id << "]";
    }
};

struct GIDHasher {
    size_t operator()(const GID &id) const {
        return static_cast<size_t>(id.id);
    }
};

GID NewGID();

void FreeGID(const GID &id);

}