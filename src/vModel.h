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
private:
    std::vector<vMesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    vMesh processMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
    unsigned int textureFromFile(const char *path, const std::string &directory, bool gamma);
public:
    vModel(const char *path)
    {
        loadModel(path);
    }
    void draw(Shader &shader) override;
    void updateSelfChildren(float deltaTime) override;
};