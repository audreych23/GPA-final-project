#pragma once

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../../externals/include/stb_image.h"
#include "../Rendering/Shader.h"

#include <vector>

namespace INANOA {
	namespace MODEL {
		class Model
		{
		public:
			Model();
			Model(char* path);
			void render();

			const std::vector<Mesh>& getMeshes() const;

		private:
			void loadModel(std::string path);

			void processNode(aiNode* node, const aiScene* scene);
			Mesh processMesh(aiMesh* mesh, const aiScene* scene);
			std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string type_name);

			unsigned int TextureFromFile(const char* path, const std::string& directory);

			std::vector<Mesh> _meshes;
			std::string _directory;
			std::vector<Texture> _textures_loaded;
		};
	}
}
