#pragma once

#include "Scene.h"

namespace pepcy::renderer {

class OBJSaver {
  public:
    void SaveScene(const std::string &directory,
        const std::string &filename, const Scene &scene);

  private:
    void SaveMTL(const Scene &scene);

    std::string name_base, directory;
};

}