#include "Trice.h"

namespace INANOA {
	namespace MODEL {
		Trice::Trice() {}

		Trice::~Trice() {}

		void Trice::init(glm::mat4 base) {
			this->m_modelMat = glm::mat4(1.0);
			this->m_modelMat = glm::scale(m_modelMat, glm::vec3(0.001, 0.001, 0.001));
			this->m_modelMat = glm::translate(m_modelMat, glm::vec3(2200.0, 650.0, -1850.0));
			this->m_modelMat = m_modelMat * base;

			this->trice_model = new MODEL::Model("models\\indoor_model\\trice.obj");
		}

		void Trice::render() {
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION,
				1, false, glm::value_ptr(this->m_modelMat));
			trice_model->render();
		}
	}
}