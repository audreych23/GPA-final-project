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
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor3"), 2);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor4"), 3);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor5"), 4);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor6"), 5);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor7"), 6);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor8"), 7);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor9"), 8);
			glUniform1i(glGetUniformLocation(m_postProcessShaderProgram->programId(), "inColor10"), 9);
			return true;

		}

		void PostProcessingBase::usePostProcessingShaderProgram() {
			this->m_postProcessShaderProgram->useProgram();
		}

	}
}