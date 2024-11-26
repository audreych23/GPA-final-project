#include "Indoor.h"

namespace INANOA {
	namespace MODEL {
		Indoor::Indoor() {}

		Indoor::~Indoor() {}

		void Indoor::init(glm::mat4 base) {
			this->m_modelMat = glm::mat4(1.0);
			this->m_modelMat =  m_modelMat * base;

			this->indoor_model = new MODEL::Model("models\\indoor_model\\Grey_White_Room.obj");
		}
		
		void Indoor::render() {
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 
				1, false, glm::value_ptr(this->m_modelMat));
			indoor_model->render();
		}
	}
}