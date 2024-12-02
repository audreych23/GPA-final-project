#include "PostProcessingBase.h"


namespace INANOA {
	namespace OPENGL {
		PostProcessingBase::PostProcessingBase() {}
		PostProcessingBase::~PostProcessingBase() {}

		bool PostProcessingBase::init(const std::string& vsResource, const std::string& fsResource, const int width, const int height) {
			this->m_postProcessShaderProgram = OPENGL::ShaderProgram::
				createShaderProgram(vsResource, fsResource);

			if (this->m_postProcessShaderProgram == nullptr) {
				return false;
			}

			this->m_postProcessShaderProgram->useProgram();

			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "screenTexture"), 0);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "blurTexture"), 1);

			return true;
		}

		void PostProcessingBase::usePostProcessingShaderProgram() {
			this->m_postProcessShaderProgram->useProgram();
		}

	}
}