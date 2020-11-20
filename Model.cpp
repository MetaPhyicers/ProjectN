#include "Model.h"
#include "MeshTech.h"
#include "Light.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Model::Model(Graphic::Renderer* renderer)
	:renderer(renderer), meshes(), meshTech(new MeshTech()), light(new Light())
{
}

Model::~Model()
{
	delete meshTech;
}

bool Model::Load(const wchar_t* filePath)
{
	std::string path = Unicode::UnicodeToMultibytes(filePath).c_str();
	directory = path.substr(0, path.find_last_of('/'));
	/** initialize mesh tech */
	if(!meshTech->Init()) {
		return false;
	}

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
		std::string excepMessage = "Exception::Model::Load(): Load model" + path + " failed!";
		throw std::invalid_argument(excepMessage.c_str());
		return false;
	}

	/**
	*	process node
	*/
	ProcessNode(scene->mRootNode, scene);

	importer.FreeScene();

	return true;
}

void Model::SetProjectionView(glm::mat4& projection)
{
	meshTech->SetProjectionView(projection);
}

void Model::SetModel(glm::mat4& model)
{
	meshTech->SetModel(model);
}

Model* Model::LoadModel(const wchar_t* filePath, Graphic::Renderer* renderer)
{
	Model* model = new Model(renderer);
	if (!model->Load(filePath)) {
		SafeDelete(model);
		return nullptr;
	}

	return model;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i : Range<unsigned int>(0, node->mNumMeshes)) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	/** process children node */
	for (unsigned int i : Range<unsigned int>(0, node->mNumChildren)) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	Mesh* newMesh = new Mesh();
	newMesh->meshTech = meshTech;

	/** 
	*	process vertices and indices
	*/
	std::vector<Mesh::Vertex> vertices;
	for (unsigned int i : Range<unsigned int>(0, mesh->mNumVertices)) {
		Mesh::Vertex vertex = {};
		/** position */
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		/** normal */
		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		if (mesh->HasTangentsAndBitangents()) {
			/** tangent */
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;
			/** bitangent */
			vertex.bitangent.x = mesh->mBitangents[i].x;
			vertex.bitangent.y = mesh->mBitangents[i].y;
			vertex.bitangent.z = mesh->mBitangents[i].z;
		}
		else {
			vertex.tangent = glm::vec3(0.f);
			vertex.bitangent = glm::vec3(0.f);
		}

		/** texture coordinate */
		if (mesh->HasTextureCoords(0)) {
			vertex.textureCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.textureCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else {
			vertex.textureCoord = glm::vec2(0.f);
		}

		vertices.push_back(vertex);
	}

	/** process indices */
	std::vector<GLuint> indices;
	for (unsigned int i : Range<unsigned int>(0, mesh->mNumFaces)) {
		aiFace& face = mesh->mFaces[i];
		for (int k : Range<int>(0, face.mNumIndices)) {
			indices.push_back(face.mIndices[k]);
		}
	}

	/** set vertices and indices to mesh */
	newMesh->SetVerticesAndIndices(vertices, indices);


	/**
	*	process textures
	*/
	auto checkTexturesRedundancy = [&](unsigned int hash)->bool {
		try
		{
			newMesh->textures.at(hash);
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	};

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		/** load diffuse textures */
		std::map<unsigned int, Mesh::Texture> diffuseMaps;
		for (unsigned int i : Range<unsigned int>(0, material->GetTextureCount(aiTextureType_DIFFUSE))) {
			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, i, &path);

			unsigned int hash = 0;
			if (path.length == 0 || checkTexturesRedundancy(hash = HashString::FNV_1A_Multibyte(path.C_Str(), path.length))) {
				continue;
			}
			else {
				Mesh::Texture texture = {};
				texture.textureID = LoadTexture(path.C_Str());
				texture.type = Mesh::TextureType::TEXTURE_DIFFUSE;
				newMesh->textures.insert(std::pair<unsigned int, Mesh::Texture>(hash, texture));
			}
		}

		/** load specular textures */
		std::map<unsigned int, Mesh::Texture> specularMaps;
		for (unsigned int i : Range<unsigned int>(0, material->GetTextureCount(aiTextureType_SPECULAR))) {
			aiString path;
			material->GetTexture(aiTextureType_SPECULAR, i, &path);

			unsigned int hash = 0;
			if (path.length == 0 || checkTexturesRedundancy(hash = HashString::FNV_1A_Multibyte(path.C_Str(), path.length))) {
				continue;
			}
			else {
				Mesh::Texture texture = {};
				texture.textureID = LoadTexture(path.C_Str());
				texture.type = Mesh::TextureType::TEXTURE_SPECULAR;
				newMesh->textures.insert(std::pair<unsigned int, Mesh::Texture>(hash, texture));
			}
		}

		/** load ambient textures... */
		
	}

	renderer->AddObeject(newMesh);
	return newMesh;
}

unsigned int Model::LoadTexture(const char* path)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);

	std::string dir = directory + '/' + path;

	unsigned char* data = stbi_load(dir.c_str(), &width, &height, &channels, 0);
	if (data) {
		GLenum format;
		switch (channels)
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		}

		GLuint textureID = 0;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		return textureID;
	}
	else {
		return 0;
	}

	stbi_image_free(data);


}

std::map<unsigned int, Mesh::Texture> Model::LoadMaterialTexture(aiMaterial* material, aiTextureType aiType, Mesh::TextureType type)
{
	std::map<unsigned int, Mesh::Texture> textures;
	for (unsigned int i : Range<unsigned int>(0, material->GetTextureCount(aiType))) {
		aiString path;
		material->GetTexture(aiType, i, &path);
		Mesh::Texture texture = {};
		texture.textureID = LoadTexture(path.C_Str());
		texture.type = type;

		textures.insert(std::pair<unsigned int, Mesh::Texture>(HashString::FNV_1A_Multibyte(path.C_Str(), path.length), texture));
	}

	return textures;
}

Mesh::Mesh()
	:textures(), meshTech(nullptr)
{
}

Mesh::~Mesh()
{
}

void Mesh::SetVerticesAndIndices(std::vector<Vertex>& vertices, std::vector<GLuint>& indices)
{
	primitive->CreateBuffer(GL_ARRAY_BUFFER);
	primitive->AttachBuffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	primitive->AttribPointer(0, 3, 14, (void*)0);
	primitive->AttribPointer(1, 3, 14, (void*)offsetof(Vertex, normal));
	primitive->AttribPointer(2, 2, 14, (void*)offsetof(Vertex, textureCoord));
	primitive->AttribPointer(3, 3, 14, (void*)offsetof(Vertex, tangent));
	primitive->AttribPointer(4, 3, 14, (void*)offsetof(Vertex, bitangent));
	primitive->DetachBuffer();

	if (!indices.empty()) {
		primitive->CreateBuffer(GL_ELEMENT_ARRAY_BUFFER);
		primitive->AttachBuffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);
		primitive->DetachBuffer();

	}
}

void Mesh::SetTextures(std::map<unsigned int, Texture>& textures)
{
	this->textures = textures;
}

bool Mesh::Update(float dt)
{
	return true;
}

bool Mesh::Render(float dt)
{
	meshTech->Use();
	int i = 0;
	for (auto& texture : textures) {
		meshTech->ActiveTexture(GL_TEXTURE0 + i);

		switch (texture.second.type)
		{
		case Mesh::TextureType::TEXTURE_DIFFUSE:
			meshTech->SetDiffuseMap(i);
			break;

		case Mesh::TextureType::TEXTURE_SPECULAR:
			meshTech->SetSpeculatMap(i);
			break;

		case Mesh::TextureType::TEXTURE_AMBIENT:
			meshTech->SetAmbientMap(i);
			break;
		}
		
		meshTech->BindTexture(texture.second.textureID);
		i++;
	}

	primitive->Render(dt);
	meshTech->ActiveTexture(GL_TEXTURE0);
	meshTech->BindTexture(0);

	return true;
}
