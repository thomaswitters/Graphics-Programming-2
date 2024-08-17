#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <glm/glm.hpp>
#include <Vertex.h>
#include <iostream>
#include <unordered_map>
#include <limits>

namespace ObjLoader {
    inline bool loadObjFile(const std::string& filename, std::vector<Vertex3D_PBR>& vertices, std::vector<uint32_t>& indices) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Could not open the file: " << filename << std::endl;
            return false;
        }

        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        size_t vertexEstimate = fileSize / 100;
        vertices.reserve(vertexEstimate);
        indices.reserve(vertexEstimate * 3);

        std::vector<glm::vec3> tempVertices;
        std::vector<glm::vec2> tempTexCoords;
        std::vector<glm::vec3> tempNormals;
        std::vector<glm::vec3> tangents;

        tempVertices.reserve(vertexEstimate);
        tempTexCoords.reserve(vertexEstimate / 2);
        tempNormals.reserve(vertexEstimate);
        tangents.reserve(vertexEstimate);

        std::unordered_map<Vertex3D_PBR, uint32_t, Vertex3D_PBR::Hash> uniqueVertices;
        uniqueVertices.reserve(vertexEstimate);

        std::vector<char> buffer(fileSize);
        file.read(buffer.data(), fileSize);

        const char* data = buffer.data();
        const char* end = data + fileSize;

        while (data < end) {
            // Skip whitespace
            while (data < end && isspace(*data)) data++;

            if (data >= end) break;

            if (*data == 'v') {
                if (data[1] == ' ') {
                    glm::vec3 vertex;
                    data += 2;
                    vertex.x = strtof(data, (char**)&data);
                    vertex.y = strtof(data, (char**)&data);
                    vertex.z = strtof(data, (char**)&data);
                    tempVertices.push_back(vertex);
                }
                else if (data[1] == 't') {
                    glm::vec2 texCoord;
                    data += 3;
                    texCoord.x = strtof(data, (char**)&data);
                    texCoord.y = 1.0f - strtof(data, (char**)&data);
                    tempTexCoords.push_back(texCoord);
                }
                else if (data[1] == 'n') {
                    glm::vec3 normal;
                    data += 3;
                    normal.x = strtof(data, (char**)&data);
                    normal.y = strtof(data, (char**)&data);
                    normal.z = strtof(data, (char**)&data);
                    tempNormals.push_back(normal);
                }
            }
            else if (*data == 'f') {
                // Face
                data++;

                int vertexIndex[3], texCoordIndex[3], normalIndex[3];
                for (int i = 0; i < 3; i++) {
                    vertexIndex[i] = strtol(data, (char**)&data, 10) - 1;
                    if (*data == '/') {
                        data++;
                        texCoordIndex[i] = strtol(data, (char**)&data, 10) - 1;
                    }
                    else {
                        texCoordIndex[i] = -1;
                    }
                    if (*data == '/') {
                        data++;
                        normalIndex[i] = strtol(data, (char**)&data, 10) - 1;
                    }
                    else {
                        normalIndex[i] = -1;
                    }
                }

                glm::vec3 positions[3];
                glm::vec2 texCoords[3];

                for (int i = 0; i < 3; ++i) {
                    if (vertexIndex[i] < 0 || vertexIndex[i] >= tempVertices.size()) {
                        std::cerr << "Vertex index out of range: " << vertexIndex[i] + 1 << std::endl;
                        return false;
                    }
                    positions[i] = tempVertices[vertexIndex[i]];

                    if (texCoordIndex[i] >= 0) {
                        if (texCoordIndex[i] >= tempTexCoords.size()) {
                            std::cerr << "Texture coordinate index out of range: " << texCoordIndex[i] + 1 << std::endl;
                            return false;
                        }
                        texCoords[i] = tempTexCoords[texCoordIndex[i]];
                    }
                    else {
                        texCoords[i] = glm::vec2(0.0f);
                    }

                    if (normalIndex[i] >= 0) {
                        if (normalIndex[i] >= tempNormals.size()) {
                            std::cerr << "Normal index out of range: " << normalIndex[i] + 1 << std::endl;
                            return false;
                        }
                    }
                }

                glm::vec3 edge1 = positions[1] - positions[0];
                glm::vec3 edge2 = positions[2] - positions[0];
                glm::vec2 deltaUV1 = texCoords[1] - texCoords[0];
                glm::vec2 deltaUV2 = texCoords[2] - texCoords[0];

                float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
                glm::vec3 tangent{
                    f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                    f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                    f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
                };

                for (int i = 0; i < 3; ++i) {
                    Vertex3D_PBR vertex{};
                    vertex.pos = positions[i];
                    vertex.normal = normalIndex[i] >= 0 ? tempNormals[normalIndex[i]] : glm::vec3(0.0f);
                    vertex.texCoord = texCoords[i];
                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    auto [it, inserted] = uniqueVertices.try_emplace(vertex, static_cast<uint32_t>(vertices.size()));
                    if (inserted) {
                        vertices.push_back(vertex);
                        indices.push_back(it->second);
                        tangents.push_back(tangent);
                    }
                    else {
                        indices.push_back(it->second);
                        tangents[it->second] += tangent;
                    }
                }
            }

            while (data < end && *data != '\n') {
                data++;
            }
            data++;
        }

#pragma omp parallel for
        for (size_t i = 0; i < vertices.size(); ++i) {
            const glm::vec3& n = vertices[i].normal;
            glm::vec3& t = tangents[i];
            vertices[i].tangent = glm::normalize(t - n * glm::dot(n, t));
        }

        return true;
    }
}