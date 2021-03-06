#pragma once

#include <string>
#include <iostream>

namespace pepcy::renderer {

const std::string root_path("C:/Users/PepcyCh/Documents/ZJU-Courses/GX-CG/toy-renderer/");
const std::string assets_path = root_path + "assets/";
const std::string scene_path = assets_path + "scenes/";
const std::string texture_path = assets_path + "textures/";
const std::string GLSLshader_path = assets_path + "GLSLshaders/";
const std::string shot_path = assets_path + "screenshots/";

// log
#define LOG(expr) std::cerr << expr << std::endl
#define CHECK(expr) std::cerr << #expr ": " << (expr) << std::endl

}
