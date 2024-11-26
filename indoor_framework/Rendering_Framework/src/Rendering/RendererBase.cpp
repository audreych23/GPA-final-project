#include "RendererBase.h"
#include "ShaderParameterBindingPoint.h"

#include <glm/gtc/type_ptr.hpp>

namespace INANOA {
	namespace OPENGL {
		RendererBase::RendererBase() {
			this->m_viewMat = glm::mat4x4(1.0f);
			this->m_projMat = glm::mat4x4(1.0f);
			this->m_viewPosition = glm::vec4(0.0f);
		}
		RendererBase::~RendererBase() {}

		bool RendererBase::init(const std::string& vsResource, const std::string& fsResource, const int width, const int height) {
			this->m_shaderProgram = ShaderProgram::createShaderProgram(vsResource, fsResource);
			if (this->m_shaderProgram == nullptr) {
				return false;
			}

			this->m_shaderProgram->useProgram();

			// added for textureBinding
			glUniform1i(glGetUniformLocation(this->m_shaderProgram->programId(), "bushTexture"), 0);
			glUniform1i(glGetUniformLocation(this->m_shaderProgram->programId(), "slimeTexture"), 1);
			// API setting
			glEnable(GL_DEPTH_TEST);
			glLineWidth(2.0f);

			//this->m_resetCSProgram = ShaderProgram::createShaderProgramForComputeShader("src\\shader\\resetShader_ogl_450.glsl");
			//if (this->m_resetCSProgram == nullptr) {
			//	return false;
			//}

			//this->m_cullingCSProgram = ShaderProgram::createShaderProgramForComputeShader("src\\shader\\computeShader_ogl_450.glsl");
			//if (this->m_cullingCSProgram == nullptr) {
			//	return false;
			//}

			return true;
		}		

		void RendererBase::setCamera(const glm::mat4& projMat, const glm::mat4& viewMat, const glm::vec3& viewOrg) {
			this->m_projMat = projMat;
			this->m_viewMat = viewMat;
			this->m_viewPosition = glm::vec4(viewOrg, 1.0f);

			//glUniform3fv(SHADER_PARAMETER_BINDING::CAMERA_POSITION, 1, &viewOrg[0]);
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::VIEW_MAT_LOCATION, 1, false, glm::value_ptr(this->m_viewMat));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::PROJ_MAT_LOCATION, 1, false, glm::value_ptr(this->m_projMat));
		}

		void RendererBase::resize(const int w, const int h) {
			this->m_frameWidth = w;
			this->m_frameHeight = h;
		}

		void RendererBase::clearRenderTarget() {
			static float DEPTH[1] = { 1.0f };
			static float COLOR[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

			glClearBufferfv(GL_COLOR, 0, COLOR);
			glClearBufferfv(GL_DEPTH, 0, DEPTH);
		}

		void RendererBase::useRenderBaseProgram() {
			this->m_shaderProgram->useProgram();
		}

		void RendererBase::useCullingCSProgram(glm::vec4 slime_pos) {
			// set camera before this, so can bind correctly
			this->m_cullingCSProgram->useProgram();
			int NUM_TOTAL_INSTANCE = 2797 + 1010 + 155304;
			glUniform1i(SHADER_PARAMETER_BINDING::NUM_TOTAL_INSTANCE, NUM_TOTAL_INSTANCE);
			//glUniform4fv(SHADER_PARAMETER_BINDING::SLIME_POS, 1, &slime_pos[0]);
			// hard coded value 
			glDispatchCompute((NUM_TOTAL_INSTANCE/ 1024) + 1, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}

		void RendererBase::useResetCSProgram() {
			this->m_resetCSProgram->useProgram();
			glDispatchCompute(3, 1, 1);
			glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		}
	}	
}
