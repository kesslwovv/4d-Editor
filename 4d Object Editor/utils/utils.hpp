//
//  utils.hpp
//  4d projection
//
//  Created by David on 19.09.22.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <random>

#include "glm/glm.hpp"

#define DIST_PATHS 1

#if DIST_PATHS
#define PATH_SHADER "Shaders/"
#define PATH_REC "Resources/"
#else
#define PATH_SHADER "../../../../4d Object Editor/Shaders/"
#define PATH_REC "../../../../4d Object Editor/Resources/"
#endif

#define DEBUG_FILE_LOAD 0

using std::string;
using std::filesystem::path;



/// Get the absolute path from path relative to executable directory
/// - Parameter relpath: Path relative to the executable directory
path abspath(const path& relpath);


/// read file relative to executable directory and return contents
/// - Parameter filepath: relative filepath
string readFile(path filepath);


/// get rference to RNG object
std::mt19937_64& getRNG();
/// get random float in range [0, 1]
float rand01();
/// get random vector with each element in range [0, 1]
glm::vec4 randColor();

#endif /* utils_hpp */
