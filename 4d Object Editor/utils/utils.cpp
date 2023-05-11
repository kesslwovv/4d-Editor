//
//  utils.cpp
//  4d projection
//
//  Created by David on 19.09.22.
//

#include "utils.hpp"


extern path executable;

path abspath(const path& relpath)
{
    path res = executable / relpath;
#if DEBUG_FILE_LOAD
    printf("executable: %s\n", executable.c_str());
	std::cout << executable << std::endl;
    printf("relpath: %s\n", relpath.c_str());
	std::cout << relpath << std::endl;
    printf("filename: %s\n", res.c_str());
	std::cout << res << std::endl;
#endif
    return res;
}

string readFile(path relpath)
{
#if DEBUG_FILE_LOAD
    printf("load: %s\n", relpath.c_str());
#endif
    path filepath = abspath(relpath);
#if DEBUG_FILE_LOAD
    printf("path: %s\n", filepath.c_str());
    printf("load: %s\n", relpath.c_str());
#endif
    const path::value_type* filename = filepath.c_str();
    
//    printf("filepath \"%s\" exists?: %d\n", filename, std::filesystem::exists(filepath));

    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        //std::cout << "read: " << filename << "\n" << contents << std::endl;
        return contents;
    }
    printf("the following path could not be opened: %s\n", filename);
    printf("errno: %d\n", errno);
    perror("error message");
//    throw(errno);
    return "";
}


std::mt19937_64& getRNG()
{
    static std::mt19937_64 RNG;
    return RNG;
}

float rand01()
{
    std::uniform_real_distribution<float> distributor {0, 1};
    
    return distributor(getRNG());
}

glm::vec4 randColor()
{
    return {
        rand01(),
        rand01(),
        rand01(),
        rand01()
    };
}
