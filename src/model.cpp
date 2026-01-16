#include "model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <filesystem>

#include <iostream>
#include <cstring>

// Local helper to create a tiny white texture as a safe fallback when a material
// is missing a diffuse map. This avoids sampling from an uninitialized texture.
namespace {
unsigned int createFallbackWhiteTexture()
{
    unsigned int tex = 0;
    glGenTextures(1, &tex);

    const unsigned char white[4] = { 255, 255, 255, 255 };

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        1,
        1,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        white
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}
}



Model::Model(const std::string& path)
{
    loadModel(path);
}



void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
}




void Model::loadModel(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    std::cout << " assimp success " << std::endl;
    std::cout << " assimp mesh:  " << scene->mNumMeshes << std::endl;
    std::cout << " assimp material: " << scene->mNumMaterials << std::endl;

    directory = std::filesystem::path(path).parent_path().string();

    processNode(scene->mRootNode, scene);
}




void Model::processNode(aiNode* node, const aiScene* scene)
{

    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }


    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}




Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;


    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // position
        vertex.Position = glm::vec3(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z
        );

        // normal
        if (mesh->HasNormals())
        {
            vertex.Normal = glm::vec3(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f);
        }

        // texcoord
        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = glm::vec2(
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            );
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f);
        }

        vertices.push_back(vertex);
    }


    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }


    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // diffuse
        std::vector<Texture> diffuseMaps =
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        // glTF 2.0 baseColor 贴图（Assimp 新版枚举）也作为漫反射处理
#ifdef aiTextureType_BASE_COLOR
        {
            std::vector<Texture> baseColorMaps =
                loadMaterialTextures(material, aiTextureType_BASE_COLOR, "texture_diffuse");
            textures.insert(textures.end(), baseColorMaps.begin(), baseColorMaps.end());
        }
#endif

        // specular
        std::vector<Texture> specularMaps =
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    // Ensure we always have at least one diffuse texture to sample. Large models
    // sometimes lack explicit diffuse maps and would otherwise sample garbage.
    const bool hasDiffuse = std::any_of(
        textures.begin(),
        textures.end(),
        [](const Texture& t) { return t.type == "texture_diffuse"; }
    );

    if (!hasDiffuse)
    {
        Texture fallback;
        fallback.id = createFallbackWhiteTexture();
        fallback.type = "texture_diffuse";
        fallback.path = "<fallback-white>";
        textures.push_back(fallback);
    }

    return Mesh(vertices, indices, textures);
}




std::vector<Texture> Model::loadMaterialTextures(
    aiMaterial* mat,
    aiTextureType type,
    const std::string& typeName
)
{
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip)
        {
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            if (texture.id == 0)
            {
                // Skip invalid textures instead of pushing a bad ID.
                continue;
            }
            texture.type = typeName;
            texture.path = str.C_Str();

            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }

    return textures;
}



unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = directory + '/' + std::string(path);

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum format;
        if (nrComponents == 1) format = GL_RED;
        else if (nrComponents == 3) format = GL_RGB;
        else format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
        return 0;
    }

    return textureID;
}
