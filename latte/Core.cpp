#include "Core.h"

#include <filesystem>
#include <string>
#include <fstream>
#define TINYOBJLOADER_IMPLEMENTATION
#include "external/tiny_obj_loader.h"
#include <iostream>

std::string Core::readFile(std::string filePath) {

    if (!std::filesystem::exists(filePath)) {
        throw std::invalid_argument(filePath + " does not exist");
    }


    std::ifstream file(filePath);
    std::string str = "";
    std::string fileContents = "";

    while (std::getline(file, str)) {

        fileContents += str;
        fileContents.push_back('\n');
    }

    return fileContents;
}


std::shared_ptr<Mesh> Core::loadMesh(std::string filePath) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str(),
        ".", false);

    if (!warn.empty()) {
        std::cout << "WARN: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }

    if (!ret) {
        std::cerr << "Failed to load " << filePath.c_str() << std::endl;
        exit(1);
    }

    std::shared_ptr<std::vector<Vertex>> vertices = std::make_shared<std::vector<Vertex>>();
    std::shared_ptr<std::vector<int>> indices = std::make_shared<std::vector<int>>();

    //Model Loading
    {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./"; // Path to material files

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filePath, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            exit(1);
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();



        // Loop over shapes
        for (size_t s = 0; s < shapes.size(); s++) {
            // Loop over faces(polygon)
            size_t indexOffset = 0;
            for (size_t faceIndex = 0; faceIndex < shapes[s].mesh.num_face_vertices.size(); faceIndex++) {
                size_t numFaceVertices = size_t(shapes[s].mesh.num_face_vertices[faceIndex]);

                // Loop over vertices in the face.
                for (size_t vertexIndex = 0; vertexIndex < numFaceVertices; vertexIndex++) {
                    // access to vertex
                    tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + vertexIndex];
                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    vertices->emplace_back(
                        glm::vec4(glm::vec3(vx, vy, vz), 1.0),
                        glm::vec3(1.0)
                    );

                    indices->push_back(indexOffset + vertexIndex);
                }
                indexOffset += numFaceVertices;
            }
        }




    }

    return std::make_shared<Mesh>(filePath, vertices, indices);
}