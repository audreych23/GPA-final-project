
#include "Model.h"

Model::Model() {}

Model::Model(char* path, char* spatial_sample_path, std::string texture_path, float texture_coord_z) : _texture_coord_z(texture_coord_z) {
	loadModel(path, texture_path);

	// load spatialsample for each model, in this current implemtnation since 
	// each model only has one mesh, a simpler method is done
	sample = sample->fromFile(spatial_sample_path);
}

//void Model::render() {
//	for (unsigned int i = 0; i < _meshes.size(); ++i) {
//		_meshes[i].render();
//	}
//}

// hope this is a fast copy
const std::vector<Mesh>& Model::getMeshes() const {
	return _meshes;
}

void Model::loadModel(std::string path, std::string texture_path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType | aiProcess_FlipUVs);
	// remember to flip the uvs

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
		return;
	}
	_directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene, texture_path);
}

void Model::processNode(aiNode* node, const aiScene* scene, std::string texture_path) {
	// process all node's meshes if it exists
	for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(processMesh(mesh, scene, texture_path)); // process the mesh into the data structure we packed
	}
	// recursively call its children
	for (unsigned int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene, texture_path);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, std::string texture_path) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		// process vertex positions, normals, and texture coordinates
		glm::vec3 vec; // temp var
		vec.x = mesh->mVertices[i].x;
		vec.y = mesh->mVertices[i].y;
		vec.z = mesh->mVertices[i].z;
		vertex.position = vec;

		// without this, some models has no normals 
		if (mesh->HasNormals()) {
			vec.x = mesh->mNormals[i].x;
			vec.y = mesh->mNormals[i].y;
			vec.z = mesh->mNormals[i].z;
			vertex.normal = vec;
		}

		// assimp allows a model to have up to 8 different textrure coordinates per vertex
		if (mesh->mTextureCoords[0]) {// does the mesh contain texture coordinates
			glm::vec3 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vec.z = _texture_coord_z;
			vertex.tex_coords = vec;
		}
		else {
			vertex.tex_coords = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	// each face represents a single primitive
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}
	
	if (texture_path != "") {
		Texture texture;
		texture.id = 0/*TextureFromFile(texture_path.C_Str(), _directory)*/;
		texture.type = "";
		texture.path = texture_path;
		textures.push_back(texture);
	}


	// process material
	/*if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		std::vector<Texture> specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}*/

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name) {
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		for (unsigned int j = 0; j < _textures_loaded.size(); ++j) {
			if (std::strcmp(_textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(_textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), _directory);
			texture.type = type_name;
			texture.path = str.C_Str();
			textures.push_back(texture);
			_textures_loaded.push_back(texture);
		}
	}
	return textures;
}

// by the way this implementation is buggy, because it actually loads the texture 2 times for bush, one in merged model, and one in normal model (here)
unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
	// right now we don't need this anyway this avoids loading the textures 2 times when we combine the model
	//std::string filename = std::string(path);
	//filename = filename;
	////std::cout << "file_name: " << filename << '\n';

	//unsigned int textureID;
	//glGenTextures(1, &textureID);

	//int width, height, nrComponents;
	//unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	//if (data)
	//{
	//	GLenum format = GL_RED;
	//	if (nrComponents == 1)
	//		format = GL_RED;
	//	else if (nrComponents == 3)
	//		format = GL_RGB;
	//	else if (nrComponents == 4)
	//		format = GL_RGBA;

	//	glBindTexture(GL_TEXTURE_2D, textureID);
	//	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//	stbi_image_free(data);
	//}
	//else
	//{
	//	std::cout << "Texture failed to load at path: " << path << std::endl;
	//	stbi_image_free(data);
	//}

	return 0;
}