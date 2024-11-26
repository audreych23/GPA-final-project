#pragma once

#include "../Rendering/Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <Rendering_Framework/src/Scene/Model.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>

#include <vector>

namespace INANOA {
	namespace MODEL {
		class Trice
		{
		public:
			Trice();
			~Trice();

			void init(glm::mat4 base);
			void render();
		private:
			glm::mat4 m_modelMat;

			Model* trice_model = nullptr;
		};
	}
}

