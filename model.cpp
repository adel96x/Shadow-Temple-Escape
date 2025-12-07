// ============================================================================
// Model.cpp - Simple OBJ Model Loader Implementation
// ============================================================================

#include "model.h"
#include <fstream>
#include <iostream>
#include <sstream>

Model::Model() {
  displayListId = 0;
  loaded = false;
  minX = minY = minZ = 1e9;
  maxX = maxY = maxZ = -1e9;
}

Model::~Model() {
  if (displayListId != 0) {
    glDeleteLists(displayListId, 1);
  }
}

bool Model::load(const char *filename) {
  Assimp::Importer importer;
  // Read file with post-processing flags
  // aiProcess_Triangulate: Convert all faces to triangles
  // aiProcess_FlipUVs: Flip texture coordinates along y-axis
  // aiProcess_GenSmoothNormals: Generate visuals normals if missing
  // aiProcess_JoinIdenticalVertices: Optimize vertices
  const aiScene *scene = importer.ReadFile(
      filename, aiProcess_Triangulate | aiProcess_FlipUVs |
                    aiProcess_GenSmoothNormals |
                    aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes |
                    aiProcess_OptimizeGraph | aiProcess_ImproveCacheLocality);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
    return false;
  }

  // Clear existing data
  vertices.clear();
  texCoords.clear();
  normals.clear();
  faces.clear();

  // Reset bounding box
  minX = minY = minZ = 1e9;
  maxX = maxY = maxZ = -1e9;

  // Process all meshes in the scene
  // For simplicity in this project, we flatten all meshes into one
  int baseVertexIndex = 0;

  for (unsigned int m = 0; m < scene->mNumMeshes; m++) {
    aiMesh *mesh = scene->mMeshes[m];

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      Vertex v;
      v.x = mesh->mVertices[i].x;
      v.y = mesh->mVertices[i].y;
      v.z = mesh->mVertices[i].z;
      vertices.push_back(v);

      // Update bounding box
      if (v.x < minX)
        minX = v.x;
      if (v.y < minY)
        minY = v.y;
      if (v.z < minZ)
        minZ = v.z;
      if (v.x > maxX)
        maxX = v.x;
      if (v.y > maxY)
        maxY = v.y;
      if (v.z > maxZ)
        maxZ = v.z;

      Normal n;
      if (mesh->HasNormals()) {
        n.x = mesh->mNormals[i].x;
        n.y = mesh->mNormals[i].y;
        n.z = mesh->mNormals[i].z;
      } else {
        n.x = 0.0f;
        n.y = 1.0f;
        n.z = 0.0f;
      }
      normals.push_back(n);

      TexCoord t;
      if (mesh->mTextureCoords[0]) {
        t.u = mesh->mTextureCoords[0][i].x;
        t.v = mesh->mTextureCoords[0][i].y;
      } else {
        t.u = 0.0f;
        t.v = 0.0f;
      }
      texCoords.push_back(t);
    }

    // Process faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
      aiFace face = mesh->mFaces[i];
      if (face.mNumIndices != 3)
        continue; // Skip non-triangles (shouldn't happen with Triangulate flag)

      Face f;
      for (int j = 0; j < 3; j++) {
        f.vIndex[j] = face.mIndices[j] + baseVertexIndex;
        f.tIndex[j] = face.mIndices[j] + baseVertexIndex;
        f.nIndex[j] = face.mIndices[j] + baseVertexIndex;
      }
      faces.push_back(f);
    }

    baseVertexIndex += mesh->mNumVertices;
  }

  // Create display list
  if (displayListId == 0) {
    displayListId = glGenLists(1);
  }
  glNewList(displayListId, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  for (const auto &face : faces) {
    for (int i = 0; i < 3; i++) {
      // Normals
      if (face.nIndex[i] < (int)normals.size()) {
        glNormal3f(normals[face.nIndex[i]].x, normals[face.nIndex[i]].y,
                   normals[face.nIndex[i]].z);
      }
      // TexCoords
      if (face.tIndex[i] < (int)texCoords.size()) {
        glTexCoord2f(texCoords[face.tIndex[i]].u, texCoords[face.tIndex[i]].v);
      }
      // Vertices
      if (face.vIndex[i] < (int)vertices.size()) {
        glVertex3f(vertices[face.vIndex[i]].x, vertices[face.vIndex[i]].y,
                   vertices[face.vIndex[i]].z);
      }
    }
  }
  glEnd();
  glEndList();

  loaded = true;
  std::cout << "Loaded model: " << filename << " (Vertices: " << vertices.size()
            << ", Faces: " << faces.size() << ")" << std::endl;
  return true;
}

void Model::render() {
  if (loaded) {
    glCallList(displayListId);
  } else {
    // Fallback if not loaded
    glutWireCube(1.0f);
  }
}
