#include "Mesh.h"

Mesh<Vertex2D> Mesh<Vertex2D>::CreateRectangle(glm::vec2 center, float width, float height, const glm::vec3& color) {
    Mesh<Vertex2D> rectangleMesh;

    std::vector<Vertex2D> vertexes{};
    std::vector<uint32_t> indices{ 0, 1, 2, 2, 3, 0 };

    // Define the four corners of the rectangle
    vertexes.push_back(Vertex2D{ {center.x - width / 2, center.y - height / 2}, color });
    vertexes.push_back(Vertex2D{ {center.x + width / 2, center.y - height / 2}, color });
    vertexes.push_back(Vertex2D{ {center.x + width / 2, center.y + height / 2}, color });
    vertexes.push_back(Vertex2D{ {center.x - width / 2, center.y + height / 2}, color });

    rectangleMesh.m_Vertices = vertexes;
    rectangleMesh.m_Indices = indices;

    return rectangleMesh;
}

Mesh<Vertex2D> Mesh<Vertex2D>::CreateRectangle(glm::vec2 center, float width, float height, const std::vector<glm::vec3>& colors) {
    Mesh<Vertex2D> rectangleMesh;

    if (colors.size() < 4) {
        glm::vec3 defaultColor = { 1.0f, 1.0f, 1.0f };
        return CreateRectangle(center, width, height, defaultColor);
    }

    rectangleMesh.m_Vertices = {
        {{center.x - width / 2, center.y - height / 2}, colors[0]},
        {{center.x + width / 2, center.y - height / 2}, colors[1]},
        {{center.x + width / 2, center.y + height / 2}, colors[2]},
        {{center.x - width / 2, center.y + height / 2}, colors[3]}
    };

    rectangleMesh.m_Indices = { 0, 1, 2, 2, 3, 0 };

    return rectangleMesh;
}

Mesh<Vertex2D> Mesh<Vertex2D>::CreateEllipse(glm::vec2 center, float width, float height, const glm::vec3& color, int nrOfVertexes) {
    Mesh<Vertex2D> ellipseMesh;
    std::vector<Vertex2D> vertexes{};
    std::vector<uint32_t> indices{};

    // Add center vertex
    vertexes.push_back({ center, color });

    // Calculate angle increment
    float angleIncrement = 2.0f * float(M_PI) / nrOfVertexes;

    // Generate points around the ellipse
    for (int i = 0; i < nrOfVertexes; ++i)
    {
        float angle = i * angleIncrement;
        float x = center.x + width * 0.5f * std::cos(angle);
        float y = center.y + height * 0.5f * std::sin(angle);
        vertexes.push_back({ glm::vec2(x, y), color });
    }

    // Create indices for the triangle fan
    for (int i = 1; i <= nrOfVertexes; ++i)
    {
        indices.push_back(0); // Center vertex
        indices.push_back(i); // Current vertex
        indices.push_back((i % nrOfVertexes) + 1); // Next vertex (wrap around)
    }

    ellipseMesh.m_Vertices = vertexes;
    ellipseMesh.m_Indices = indices;

    return ellipseMesh;
}


Mesh<Vertex2D> Mesh<Vertex2D>::CreateEllipse(glm::vec2 center, float width, float height, const glm::vec3& innerColor, const glm::vec3& outerColor, int nrOfVertexes) {
    Mesh<Vertex2D> ellipseMesh;
    std::vector<Vertex2D> vertexes{};
    std::vector<uint32_t> indices{};

    // Add center vertex with inner color
    vertexes.push_back({ center, innerColor });

    // Calculate angle increment
    float angleIncrement = 2.0f * float(M_PI) / nrOfVertexes;

    // Generate points around the ellipse with outer color
    for (int i = 0; i < nrOfVertexes; ++i)
    {
        float angle = i * angleIncrement;
        float x = center.x + width * 0.5f * std::cos(angle);
        float y = center.y + height * 0.5f * std::sin(angle);
        vertexes.push_back({ glm::vec2(x, y), outerColor });
    }

    // Create indices for the triangle fan
    for (int i = 1; i <= nrOfVertexes; ++i)
    {
        indices.push_back(0); // Center vertex
        indices.push_back(i); // Current outer vertex
        indices.push_back((i % nrOfVertexes) + 1); // Next outer vertex (wrap around)
    }

    ellipseMesh.m_Vertices = vertexes;
    ellipseMesh.m_Indices = indices;

    return ellipseMesh;
}

