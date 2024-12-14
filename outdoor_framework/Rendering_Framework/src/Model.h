#pragma once

#include <iostream>
#include <vector>
#include "Mesh.h"

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include "stb_image.h"
#include "MyPoissonSample.h"

class Model
{
public:
	Model();
	Model(char* path, char* spatial_sample_path, std::string texture_path = "", float texture_coord_z = 0.0f);
	//void render();

	const std::vector<Mesh>& getMeshes() const;

	int getNumSamples() const { return sample->m_numSample; }

	const float* getInstancePositions(const int idx) const { return sample->m_positions + idx * 3; }
	const float* getInstanceRadians(const int idx) const { return sample->m_radians + idx * 3; }
private:
	void loadModel(std::string path, std::string texture_path);

	void processNode(aiNode* node, const aiScene* scene, std::string texture_path);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, std::string texture_path);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name);

	unsigned int TextureFromFile(const char* path, const std::string& directory);

	std::vector<Mesh> _meshes;
	std::string _directory;
	std::vector<Texture> _textures_loaded;

	MyPoissonSample* sample = nullptr;
	// a little bit of hacks for 
	float _texture_coord_z;
};
