// ============================================================================
// Model.h - Simple OBJ Model Loader
// Handles loading and rendering of .obj files with texture coordinates
// ============================================================================

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Assimp includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Vertex {
    float x, y, z;
};

struct TexCoord {
    float u, v;
};

struct Normal {
    float x, y, z;
};

struct Face {
    int vIndex[3];
    int tIndex[3];
    int nIndex[3];
};

class Model {
private:
    std::vector<Vertex> vertices;
    std::vector<TexCoord> texCoords;
    std::vector<Normal> normals;
    std::vector<Face> faces;
    
    GLuint displayListId;
    bool loaded;
    
    // Bounding box
    float minX, minY, minZ;
    float maxX, maxY, maxZ;

public:
    Model();
    ~Model();

    bool load(const char* filename);
    void render();
    
    // Get dimensions
    float getWidth() const { return maxX - minX; }
    float getHeight() const { return maxY - minY; }
    float getDepth() const { return maxZ - minZ; }
};

#endif // MODEL_H
