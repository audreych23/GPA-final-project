#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <Rendering_Framework/src/Scene/Model.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>

#include <vector>

namespace INANOA {
	namespace MODEL {
		class LightSphere
		{
		public:
			LightSphere();
			~LightSphere();

			void init(glm::mat4 base);

			void render();

			const glm::mat4 getModelMat() const { return m_modelMat; }
		private:
			glm::mat4 m_modelMat;

			MODEL::Model* sphere_model = nullptr;
		};
	}
}


