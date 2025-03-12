#pragma once
#include <memory>
#include <string>
#include "core/Mesh.h"


class Core {
public:
    static std::string readFile(std::string filePath);
    static std::shared_ptr<Mesh> loadMesh(std::string filePath);
    static std::shared_ptr<std::vector<char>> readBinaryFile(std::string filePath);

};

