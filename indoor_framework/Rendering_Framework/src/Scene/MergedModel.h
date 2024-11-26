#pragma once
#include <Rendering_Framework/src/Scene/Model.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <glm/gtc/type_ptr.hpp>

#include "../Rendering/Shader.h"

namespace INANOA {
	namespace MODEL {

		struct DrawElementsIndirectCommand {
			// number of index need to draw
			GLuint count;
			// number of instance
			GLuint instanceCount;
			// location of the first index
			GLuint firstIndex;
			// a constant that is added to each index
			GLuint baseVertex;
			// a base instance for fetching instanced vertex attributes
			GLuint baseInstance;
		};

		struct InstanceProperties {
			glm::vec4 position;
			glm::vec4 is_eaten; // alignment issue if not vec4
		};

		struct InstancePropertiesOut {
			glm::vec4 position;
		};

		class MergedModel
		{
		public:
			MergedModel();

			~MergedModel();
		public:
			void init();

			void initialize();

			void render();
		private:
			void mergeModel();

			void loadMergedTextureFromFile();

			Model* m_model_bush_01 = nullptr;
			Model* m_model_bush_05 = nullptr;
			Model* m_model_grass = nullptr;
			

			GLuint _vao, _vbo, _ebo, _cmd_buffer_handle, 
				_instance_buffer, _texture_array_handler;

			glm::mat4 m_modelMat;
		};
	}
}

