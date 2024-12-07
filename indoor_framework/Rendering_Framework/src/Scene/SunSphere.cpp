#include "SunSphere.h"

namespace INANOA {
	namespace MODEL {
		SunSphere::SunSphere() {}

		SunSphere::~SunSphere() {}

		void SunSphere::init(glm::mat4 base) {
			this->m_base = base;

			this->sphere_model = new MODEL::Model("models\\indoor_model\\Sphere.obj");
		}

		void SunSphere::render(glm::vec3 pos) {
			this->m_modelMat = glm::mat4(1.0);
			this->m_modelMat = glm::translate(m_modelMat, pos);
			this->m_modelMat = glm::scale(m_modelMat, glm::vec3(0.5, 0.5, 0.5));
			this->m_modelMat = m_modelMat * m_base;

			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION,
				1, false, glm::value_ptr(this->m_modelMat));

			sphere_model->render();
		}
	}
}