#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

struct ObjVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

namespace ObjLoader {
    inline bool loadObjFile(const std::string& filename, std::vector<ObjVertex>& vertices, std::vector<uint32_t>& indices) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::ivec3> faceIndices;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            iss >> token;
            if (token == "v") {
                glm::vec3 position;
                iss >> position.x >> position.y >> position.z;
                positions.push_back(position);
            }
            else if (token == "vn") {
                glm::vec3 normal;
                iss >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            }
            else if (token == "f") {
                glm::ivec3 faceIndex;
                char slash;
                for (int i = 0; i < 3; ++i) {
                    iss >> faceIndex[i] >> slash;
                    faceIndex[i]--;
                }
                faceIndices.push_back(faceIndex);
            }
        }

        file.close();

        vertices.clear();
        indices.clear();
        for (const auto& faceIndex : faceIndices) {
            ObjVertex vertex;
            vertex.position = positions[faceIndex.x];
            vertex.normal = normals[faceIndex.z];
            vertices.push_back(vertex);
            indices.push_back(static_cast<uint32_t>(vertices.size()) - 1);
        }

        return true;
    }
}