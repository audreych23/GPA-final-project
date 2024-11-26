#include "MergedModel.h"

namespace INANOA {
	namespace MODEL {
		MergedModel::MergedModel() {}
			
		MergedModel::~MergedModel() {}

		void MergedModel::init() {
			this->m_model_bush_01 = new MODEL::Model("models\\foliages\\bush01_lod2.obj", "models\\spatialSamples\\poissonPoints_1010s.ss2", 0.0f);
			this->m_model_bush_05 = new MODEL::Model("models\\foliages\\bush05_lod2.obj", "models\\spatialSamples\\poissonPoints_2797s.ss2", 1.0f);
			this->m_model_grass = new MODEL::Model("models\\foliages\\grassB.obj", "models\\spatialSamples\\poissonPoints_155304s.ss2", 2.0f);

			//this->_render_program = 

			/*this->_reset_compute_shader = OPENGL::ShaderProgram::createShaderProgramForComputeShader("src\\shader\\resetShader_ogl_450.glsl");
			if (this->_reset_compute_shader == nullptr) {
				std::cout << "error loading compute shader reset";
			}

			this->_culling_compute_shader = OPENGL::ShaderProgram::createShaderProgramForComputeShader("src\\shader\\computeShader_ogl_450");
			if (this->_culling_compute_shader == nullptr) {
				std::cout << "error loading compute shader reset";
			}*/

			//this->m_shaderProgram->useProgram();
			
			//mergeModel();
			initialize();
			loadMergedTextureFromFile();
			this->m_modelMat = glm::mat4(1.0f);
		}

		void MergedModel::render() {
			// render texture binded in GL_TEXTURE0;
			glActiveTexture(GL_TEXTURE0);
			////glUniform1i(SHADER_PARAMETER_BINDING::TEXTURE_LOCATION, 0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, _texture_array_handler);

			glBindVertexArray(_vao);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _cmd_buffer_handle);
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
			glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, 3, 0);
			glBindVertexArray(0);

			glActiveTexture(GL_TEXTURE0);
		}

		void MergedModel::mergeModel() {
			auto model_mesh_1 = m_model_bush_01->getMeshes();
			auto model_mesh_2 = m_model_bush_05->getMeshes();
			auto model_mesh_3 = m_model_grass->getMeshes();

			// since actually one object only has one mesh, this problem becomes simpler where 
			// we only have to copy all positions, normals and meshses in one go to a new vector
			glGenBuffers(1, &_cmd_buffer_handle);
			glGenVertexArrays(1, &_vao);
			glGenBuffers(1, &_vbo);
			glGenBuffers(1, &_instance_buffer);
			glGenBuffers(1, &_ebo);

			// for Indirect draw
			DrawElementsIndirectCommand drawCommands[3];
			// first bush model
			drawCommands[0].count = model_mesh_1[0].indices.size();
			drawCommands[0].instanceCount = m_model_bush_01->getNumSamples();
			drawCommands[0].firstIndex = 0;
			drawCommands[0].baseVertex = 0;
			drawCommands[0].baseInstance = 0 * sizeof(float);

			// second bush model
			drawCommands[1].count = model_mesh_2[0].indices.size();
			drawCommands[1].instanceCount = m_model_bush_05->getNumSamples();
			drawCommands[1].firstIndex = model_mesh_1[0].indices.size();
			drawCommands[1].baseVertex = model_mesh_1[0].vertices.size();
			drawCommands[1].baseInstance = m_model_bush_01->getNumSamples();

			// third bush model
			drawCommands[2].count = model_mesh_3[0].indices.size();
			drawCommands[2].instanceCount = m_model_grass->getNumSamples();
			drawCommands[2].firstIndex = model_mesh_1[0].indices.size() + model_mesh_2[0].indices.size();
			drawCommands[2].baseVertex = model_mesh_1[0].vertices.size() + model_mesh_2[0].vertices.size();
			drawCommands[2].baseInstance = m_model_bush_01->getNumSamples() + m_model_bush_05->getNumSamples();

			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _cmd_buffer_handle);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommands), drawCommands, GL_DYNAMIC_DRAW);

			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			// allocate this much data in the buffer

			glBufferData(GL_ARRAY_BUFFER, 
				(model_mesh_1[0].vertices.size() + 
				model_mesh_2[0].vertices.size() + 
				model_mesh_3[0].vertices.size()) *
				sizeof(Vertex), NULL, GL_STATIC_DRAW);

			GLuint offset = 0;
			glBufferSubData(GL_ARRAY_BUFFER, offset,
				model_mesh_1[0].vertices.size() * sizeof(Vertex), model_mesh_1[0].vertices.data());

			offset += (model_mesh_1[0].vertices.size() * sizeof(Vertex));

			glBufferSubData(GL_ARRAY_BUFFER, offset,
				model_mesh_2[0].vertices.size() * sizeof(Vertex), model_mesh_2[0].vertices.data());

			offset += (model_mesh_2[0].vertices.size() * sizeof(Vertex));

			glBufferSubData(GL_ARRAY_BUFFER, offset,
				model_mesh_3[0].vertices.size() * sizeof(Vertex), model_mesh_3[0].vertices.data());
			
			// bind the indices that we want to draw
			offset = 0;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
				(model_mesh_1[0].indices.size() + 
				model_mesh_2[0].indices.size() + 
				model_mesh_3[0].indices.size()) * 
				sizeof(unsigned int), NULL, GL_STATIC_DRAW);

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset,
				model_mesh_1[0].indices.size() * sizeof(unsigned int), model_mesh_1[0].indices.data());

			offset += (model_mesh_1[0].indices.size() * sizeof(unsigned int));

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset,
				model_mesh_2[0].indices.size() * sizeof(unsigned int), model_mesh_2[0].indices.data());

			offset += (model_mesh_2[0].indices.size() * sizeof(unsigned int));

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset,
				model_mesh_3[0].indices.size() * sizeof(unsigned int), model_mesh_3[0].indices.data());

			// vertex positions for vao
			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::VERTEX_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::NORMAL_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

			glBindBuffer(GL_ARRAY_BUFFER, _instance_buffer);
			glBufferData(GL_ARRAY_BUFFER, 
				(m_model_bush_01->getNumSamples() +
				m_model_bush_05->getNumSamples() +
				m_model_grass->getNumSamples()) * 3 * 2 * 
				sizeof(float), NULL, GL_STATIC_DRAW);
			
			offset = 0;
			glBufferSubData(GL_ARRAY_BUFFER, offset, 
				m_model_bush_01->getNumSamples() * 3 * 2 * sizeof(float), &m_model_bush_01->getInstancePositions(0)[0]);

			offset += (m_model_bush_01->getNumSamples() * 3 * 2 * sizeof(float));

			glBufferSubData(GL_ARRAY_BUFFER, offset,
				m_model_bush_05->getNumSamples() * 3 * 2 * sizeof(float), &m_model_bush_05->getInstancePositions(0)[0]);

			offset += (m_model_bush_05->getNumSamples() * 3 * 2 * sizeof(float));

			glBufferSubData(GL_ARRAY_BUFFER, offset,
				m_model_grass->getNumSamples() * 3 * 2 * sizeof(float), &m_model_grass->getInstancePositions(0)[0]);


			// Instance Buffer
			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glVertexAttribDivisor(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION, 1);


			// bind indirect buffer
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _cmd_buffer_handle);
			glBindVertexArray(0);
			
			
		}


		void MergedModel::loadMergedTextureFromFile()
		{
			auto model_mesh_1 = m_model_bush_01->getMeshes();
			auto model_mesh_2 = m_model_bush_05->getMeshes();
			auto model_mesh_3 = m_model_grass->getMeshes();
			// luckily there's only 1 texture and 1 mesh
			std::string path_0 = model_mesh_1[0].textures[0].path;
			std::string path_1 = model_mesh_2[0].textures[0].path;
			std::string path_2 = model_mesh_3[0].textures[0].path;

			// 3 different models filename
			std::string filename_0 = "textures/" + std::string(path_0);
			std::string filename_1 = "textures/" + std::string(path_1);
			std::string filename_2 = "textures/" + std::string(path_2);
			//filename = "textures/" + filename;
			//std::cout << "file_name:\n " << filename_0 << '\n';

			int width, height, nrComponents;
			// get different texture 
			unsigned char* data_0 = stbi_load(filename_0.c_str(), &width, &height, &nrComponents, 0);
			unsigned char* data_1 = stbi_load(filename_1.c_str(), &width, &height, &nrComponents, 0);
			unsigned char* data_2 = stbi_load(filename_2.c_str(), &width, &height, &nrComponents, 0);

			const int NUM_TEXTURE = 3;
			const int IMG_WIDTH = 1024;
			const int IMG_HEIGHT = 1024;
			const int IMG_CHANNEL = 4;

			unsigned char* texture_array_data = new unsigned char[IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * NUM_TEXTURE];

			// memcpy(void* dest, const void* source, size_t n);
			memcpy(texture_array_data, data_0, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);
			memcpy(texture_array_data + IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL, data_1, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);
			memcpy(texture_array_data + IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * 2, data_2, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);

			if (data_0 && data_1 && data_2)
			{
				glGenTextures(1, &_texture_array_handler);
				glBindTexture(GL_TEXTURE_2D_ARRAY, _texture_array_handler);
				// the internal format for glTexStorageXD must be "Sized Internal Formats“
				// max mipmap level = log2(1024) + 1 = 11
				glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, IMG_WIDTH, IMG_HEIGHT, NUM_TEXTURE);
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 
					IMG_WIDTH, IMG_HEIGHT, NUM_TEXTURE, GL_RGBA, GL_UNSIGNED_BYTE, texture_array_data);

				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

				//IMPORTANT !! Use mipmap for the foliage rendering
				glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

				stbi_image_free(data_0);
				stbi_image_free(data_1);
				stbi_image_free(data_2);
			}
			else
			{
				if (!data_0) {
					std::cout << "Texture failed to load at path: " << filename_0 << std::endl;
					stbi_image_free(data_0);
				}

				if (!data_1) {
					std::cout << "Texture failed to load at path: " << filename_1 << std::endl;
					stbi_image_free(data_1);
				}

				if (!data_2) {
					std::cout << "Texture failed to load at path: " << filename_2 << std::endl;
					stbi_image_free(data_2);
				}
			}
		}

		void MergedModel::initialize() {
			auto model_mesh_1 = m_model_bush_01->getMeshes();
			auto model_mesh_2 = m_model_bush_05->getMeshes();
			auto model_mesh_3 = m_model_grass->getMeshes();

			// since actually one object only has one mesh, this problem becomes simpler where 
			// we only have to copy all positions, normals and meshses in one go to a new vector
			glGenVertexArrays(1, &_vao);
			glBindVertexArray(_vao);

			glGenBuffers(1, &_vbo);
			glGenBuffers(1, &_instance_buffer);
			glGenBuffers(1, &_ebo);

			// SSBO for raw instance and valid instance data
			int NUM_TOTAL_INSTANCE = m_model_bush_01->getNumSamples() + m_model_bush_05->getNumSamples() + m_model_grass->getNumSamples();
			// initialize raw instance data
			InstanceProperties* raw_ins_data = new InstanceProperties[NUM_TOTAL_INSTANCE];

			// get position 
			for (int i = 0; i < NUM_TOTAL_INSTANCE; ++i) {
				if (i >= m_model_bush_01->getNumSamples() &&
					i < (m_model_bush_05->getNumSamples() + m_model_bush_01->getNumSamples())) {
					const float* inst_pos = m_model_bush_05->getInstancePositions(i - m_model_bush_01->getNumSamples());
					raw_ins_data[i].position = glm::vec4(inst_pos[0], inst_pos[1], inst_pos[2], 1.0f);
				}
				else if (i >= (m_model_bush_05->getNumSamples() + m_model_bush_01->getNumSamples())) {
					const float* inst_pos = m_model_grass->getInstancePositions(i - (m_model_bush_01->getNumSamples() + m_model_bush_05->getNumSamples()));
					raw_ins_data[i].position = glm::vec4(inst_pos[0], inst_pos[1], inst_pos[2], 2.0f);
				}
				else {
					const float* inst_pos = m_model_bush_01->getInstancePositions(i);
					raw_ins_data[i].position = glm::vec4(inst_pos[0], inst_pos[1], inst_pos[2], 0.0f);
				}
				raw_ins_data[i].is_eaten = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			}
			// prepare a SSBO for storing raw instance data
			GLuint raw_instance_data_buffer_handle;
			glGenBuffers(1, &raw_instance_data_buffer_handle);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, raw_instance_data_buffer_handle);
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_TOTAL_INSTANCE * sizeof(InstanceProperties), raw_ins_data, GL_MAP_READ_BIT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, raw_instance_data_buffer_handle);

			// prepare a SSBO for storing VALID instance data
			GLuint valid_instance_data_buffer_handle;
			glGenBuffers(1, &valid_instance_data_buffer_handle);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, valid_instance_data_buffer_handle);
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_TOTAL_INSTANCE * sizeof(InstancePropertiesOut), nullptr, GL_MAP_READ_BIT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, valid_instance_data_buffer_handle);

			// for Indirect draw
			DrawElementsIndirectCommand drawCommands[3];
			// first bush model
			drawCommands[0].count = model_mesh_1[0].indices.size();
			drawCommands[0].instanceCount = m_model_bush_01->getNumSamples();
			drawCommands[0].firstIndex = 0;
			drawCommands[0].baseVertex = 0;
			drawCommands[0].baseInstance = 0 * sizeof(float);

			// second bush model
			drawCommands[1].count = model_mesh_2[0].indices.size();
			drawCommands[1].instanceCount = m_model_bush_05->getNumSamples();
			drawCommands[1].firstIndex = model_mesh_1[0].indices.size();
			drawCommands[1].baseVertex = model_mesh_1[0].vertices.size();
			drawCommands[1].baseInstance = m_model_bush_01->getNumSamples();

			// third bush model
			drawCommands[2].count = model_mesh_3[0].indices.size();
			drawCommands[2].instanceCount = m_model_grass->getNumSamples();
			drawCommands[2].firstIndex = model_mesh_1[0].indices.size() + model_mesh_2[0].indices.size();
			drawCommands[2].baseVertex = model_mesh_1[0].vertices.size() + model_mesh_2[0].vertices.size();
			drawCommands[2].baseInstance = m_model_bush_01->getNumSamples() + m_model_bush_05->getNumSamples();

			// SSBO for command_buffer_handle
			//GLuint cmd_buffer_handle;
			glGenBuffers(1, &_cmd_buffer_handle);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, _cmd_buffer_handle);
			glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(DrawElementsIndirectCommand) * 3,
				drawCommands, GL_MAP_READ_BIT);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _cmd_buffer_handle);
			
			//glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _cmd_buffer_handle);
			//glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(drawCommands), drawCommands, GL_DYNAMIC_DRAW);


			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			// allocate this much data in the buffer

			glBufferData(GL_ARRAY_BUFFER,
				(model_mesh_1[0].vertices.size() +
					model_mesh_2[0].vertices.size() +
					model_mesh_3[0].vertices.size()) *
				sizeof(Vertex), NULL, GL_STATIC_DRAW);

			GLuint offset = 0;
			glBufferSubData(GL_ARRAY_BUFFER, offset,
				model_mesh_1[0].vertices.size() * sizeof(Vertex), model_mesh_1[0].vertices.data());

			offset += (model_mesh_1[0].vertices.size() * sizeof(Vertex));

			glBufferSubData(GL_ARRAY_BUFFER, offset,
				model_mesh_2[0].vertices.size() * sizeof(Vertex), model_mesh_2[0].vertices.data());

			offset += (model_mesh_2[0].vertices.size() * sizeof(Vertex));

			glBufferSubData(GL_ARRAY_BUFFER, offset,
				model_mesh_3[0].vertices.size() * sizeof(Vertex), model_mesh_3[0].vertices.data());

			// bind the indices that we want to draw
			offset = 0;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				(model_mesh_1[0].indices.size() +
					model_mesh_2[0].indices.size() +
					model_mesh_3[0].indices.size()) *
				sizeof(unsigned int), NULL, GL_STATIC_DRAW);

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset,
				model_mesh_1[0].indices.size() * sizeof(unsigned int), model_mesh_1[0].indices.data());

			offset += (model_mesh_1[0].indices.size() * sizeof(unsigned int));
		
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset,
				model_mesh_2[0].indices.size() * sizeof(unsigned int), model_mesh_2[0].indices.data());

			offset += (model_mesh_2[0].indices.size() * sizeof(unsigned int));

			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset,
				model_mesh_3[0].indices.size() * sizeof(unsigned int), model_mesh_3[0].indices.data());

			// vertex positions for vao
			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::VERTEX_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::NORMAL_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

			// use SSBO as vertex attribute input
			glBindBuffer(GL_ARRAY_BUFFER, valid_instance_data_buffer_handle);
			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION, 4, GL_FLOAT, false, 0, (void*)0);
			glVertexAttribDivisor(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION, 1);

			// done
			glBindVertexArray(0);
		}
	}
}