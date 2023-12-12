#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <iostream>

#include "vMesh.h"
#include "GraphNode.h"
#include "Shader.h"

#include "stb_image.h"

class vModel : public GraphNode
{
protected:
    std::string directory;
    float size;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    vMesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    unsigned int textureFromFile(const char *path, const std::string &directory, bool gamma);
public:
    std::vector<vMesh> meshes;
    std::vector<Texture> textures_loaded;
    vModel(const char *path, float size);
    virtual void draw(Shader &shader) override;
    void updateSelfChildren(float deltaTime) override;
};