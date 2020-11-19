#pragma once
#include "Utility.h"
#include "Renderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment(lib, "assimp-vc142-mt.lib")

class Mesh : public Graphic::RenderTarget
{
public:
	Mesh();
	~Mesh();

private:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 textureCoord;
		glm::vec3 tangent;
		glm::vec3 bitangent;
	};
	enum class TextureType
	{
		TEXTURE_DIFFUSE,
		TEXTURE_SPECULAR,
		TEXTURE_AMBIENT,
		TEXTURE_HIGHTMAP
	};
	struct Texture
	{
		GLuint textureID;
		TextureType type;
	};

	std::map<unsigned int, Texture> textures;

	class MeshTech* meshTech;

	void SetVerticesAndIndices(std::vector<Vertex>& vertices, std::vector<GLuint>& indices);
	void SetTextures(std::map<unsigned int, Texture>& textures);

	bool Update(float dt);
	bool Render(float dt);
	
	friend class Model;
};

class Model 
{
public:
	Model(Graphic::Renderer* renderer);
	~Model();

	bool Load(const wchar_t* filePath);
	void SetProjectionView(glm::mat4& projection);
	void SetModel(glm::mat4& model);

	static Model* LoadModel(const wchar_t* filePath, Graphic::Renderer* renderer);

private:
	Graphic::Renderer* renderer;
	std::vector<Mesh*> meshes;

	std::string directory;
	class MeshTech* meshTech;

	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
	unsigned int LoadTexture(const char* path);
	std::map<unsigned int, Mesh::Texture> LoadMaterialTexture(aiMaterial* material, aiTextureType aiType, Mesh::TextureType type);
};

