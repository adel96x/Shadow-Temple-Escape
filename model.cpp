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
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Failed to open model file: " << filename << std::endl;
    return false;
  }

  std::string line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#')
      continue;

    std::stringstream ss(line);
    std::string prefix;
    ss >> prefix;

    if (prefix == "v") {
      Vertex v;
      ss >> v.x >> v.y >> v.z;
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

    } else if (prefix == "vt") {
      TexCoord t;
      ss >> t.u >> t.v;
      texCoords.push_back(t);
    } else if (prefix == "vn") {
      Normal n;
      ss >> n.x >> n.y >> n.z;
      normals.push_back(n);
    } else if (prefix == "f") {
      Face f;
      char slash;
      for (int i = 0; i < 3; i++) {
        ss >> f.vIndex[i];

        // Handle different formats: v, v//vn, v/vt, v/vt/vn
        if (ss.peek() == '/') {
          ss >> slash;
          if (ss.peek() == '/') {
            // v//vn
            ss >> slash >> f.nIndex[i];
            f.tIndex[i] = 0;
          } else {
            // v/vt or v/vt/vn
            ss >> f.tIndex[i];
            if (ss.peek() == '/') {
              ss >> slash >> f.nIndex[i];
            } else {
              f.nIndex[i] = 0;
            }
          }
        } else {
          f.tIndex[i] = 0;
          f.nIndex[i] = 0;
        }

        // OBJ indices are 1-based
        f.vIndex[i]--;
        if (f.tIndex[i] > 0)
          f.tIndex[i]--;
        if (f.nIndex[i] > 0)
          f.nIndex[i]--;
      }
      faces.push_back(f);
    }
  }

  file.close();

  // Create display list for performance
  displayListId = glGenLists(1);
  glNewList(displayListId, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  for (const auto &face : faces) {
    for (int i = 0; i < 3; i++) {
      if (!normals.empty() && face.nIndex[i] >= 0 &&
          face.nIndex[i] < (int)normals.size()) {
        glNormal3f(normals[face.nIndex[i]].x, normals[face.nIndex[i]].y,
                   normals[face.nIndex[i]].z);
      }

      if (!texCoords.empty() && face.tIndex[i] >= 0 &&
          face.tIndex[i] < (int)texCoords.size()) {
        glTexCoord2f(texCoords[face.tIndex[i]].u, texCoords[face.tIndex[i]].v);
      }

      if (face.vIndex[i] >= 0 && face.vIndex[i] < (int)vertices.size()) {
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
