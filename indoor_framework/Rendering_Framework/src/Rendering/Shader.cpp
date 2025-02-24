#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace INANOA {
	namespace OPENGL {
		Shader::Shader(const GLenum shaderType) : m_shaderType(shaderType), m_shaderId(0) {
			this->m_shaderStatus = ShaderStatus::NULL_SHADER_CODE;
		}
		Shader::~Shader() {
			this->releaseShader();
		}
		void Shader::appendShaderCode(const std::string& code) {
			this->m_shaderCode = code;
			this->m_shaderStatus = ShaderStatus::NULL_SHADER;
		}
		bool Shader::compileShader() {
			if (this->status() != ShaderStatus::NULL_SHADER) {
				return false;
			}

			this->m_shaderId = glCreateShader(this->m_shaderType);
			const char* shaderCode = this->m_shaderCode.c_str();
			glShaderSource(this->m_shaderId, 1, &shaderCode, nullptr);
			glCompileShader(this->m_shaderId);

			// check compilation errors
			int params[2];
			glGetShaderiv(this->m_shaderId, GL_COMPILE_STATUS, params);

			// compile failed
			if (params[0] != GL_TRUE) {
				char compileError[1024];
				glGetShaderInfoLog(this->m_shaderId, 1024, nullptr, compileError);
				this->m_shaderInfoLog = compileError;
				return false;
			}
			// compile success	
			this->m_shaderInfoLog = "ready";
			this->m_shaderStatus = ShaderStatus::READY;

			return true;
		}
		void Shader::releaseShader() {
			if (this->status() != ShaderStatus::READY) {
				return;
			}
			glDeleteShader(this->m_shaderId);
		}
		bool Shader::createShaderFromFile(const std::string& fileFullpath) {
			// read shader code from file
			std::ifstream inputStream;
			inputStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

			std::string shaderCode;
			try {
				inputStream.open(fileFullpath);
				std::stringstream shaderCodeStream;
				shaderCodeStream << inputStream.rdbuf();
				inputStream.close();

				shaderCode = shaderCodeStream.str();
			}
			catch (std::ifstream::failure& e) {
				this->m_shaderInfoLog = e.what();
				this->m_shaderStatus = ShaderStatus::NULL_SHADER_CODE;
				return false;
			}

			// append shader code
			this->appendShaderCode(shaderCode);

			// compile shader
			return this->compileShader();
		}


		// ========================================================
		ShaderProgram::ShaderProgram() : m_programId(0u) {
			this->m_shaderProgramStatus = ShaderProgramStatus::NULL_VERTEX_SHADER_FRAGMENT_SHADER;
			this->m_vsReady = false;
			this->m_fsReady = false;
			this->m_csReady = false;
		}
		ShaderProgram::~ShaderProgram() {}
		bool ShaderProgram::init() {
			this->m_programId = glCreateProgram();

			// error occurs
			if (this->m_programId <= 0) {
				return false;
			}

			this->m_shaderProgramStatus = ShaderProgramStatus::PROGRAM_ID_READY;
			return true;
		}
		bool ShaderProgram::attachShader(const Shader* shader) {
			if (this->m_shaderProgramStatus != ShaderProgramStatus::PROGRAM_ID_READY) {
				return false;
			}
			if (shader->status() != ShaderStatus::READY) {
				return false;
			}
			glAttachShader(this->m_programId, shader->shaderId());

			if (shader->shaderType() == GL_VERTEX_SHADER) {
				this->m_vsReady = true;
			}
			else if (shader->shaderType() == GL_FRAGMENT_SHADER) {
				this->m_fsReady = true;
			}
			else if (shader->shaderType() == GL_COMPUTE_SHADER) {
				this->m_csReady = true;
			}
			return true;
		}
		ShaderProgramStatus ShaderProgram::checkStatus() {
			// compute shader program
			if (this->m_csReady == true) {
				this->m_shaderProgramStatus = ShaderProgramStatus::READY;
			}
			// render program
			else if (this->m_vsReady == false && this->m_fsReady == false) {
				this->m_shaderProgramStatus = ShaderProgramStatus::NULL_VERTEX_SHADER_FRAGMENT_SHADER;
			}
			else if (this->m_vsReady == false) {
				this->m_shaderProgramStatus = ShaderProgramStatus::NULL_VERTEX_SHADER;
			}
			else if (this->m_fsReady == false) {
				this->m_shaderProgramStatus = ShaderProgramStatus::NULL_FRAGMENT_SHADER;
			}
			else {
				this->m_shaderProgramStatus = ShaderProgramStatus::READY;
			}

			return this->m_shaderProgramStatus;
		}
		void ShaderProgram::linkProgram() {
			if (this->m_shaderProgramStatus != ShaderProgramStatus::READY) {
				return;
			}

			glLinkProgram(this->m_programId);
		}

		// ======================================
		void ShaderProgram::setBool(const std::string& name, bool value) const {
			glUniform1i(glGetUniformLocation(this->m_programId, name.c_str()), (int)value);
		}

		void ShaderProgram::setInt(const std::string& name, int value) const {
			glUniform1i(glGetUniformLocation(this->m_programId, name.c_str()), value);
		}

		void ShaderProgram::setFloat(const std::string& name, float value) const {
			glUniform1f(glGetUniformLocation(this->m_programId, name.c_str()), value);
		}

		void ShaderProgram::setVec2(const std::string& name, const glm::vec2& value) const {
			glUniform2fv(glGetUniformLocation(this->m_programId, name.c_str()), 1, &value[0]);
		}

		void ShaderProgram::setVec2(const std::string& name, float x, float y) const {
			glUniform2f(glGetUniformLocation(this->m_programId, name.c_str()), x, y);
		}

		void ShaderProgram::setVec3(const std::string& name, const glm::vec3& value) const {
			glUniform3fv(glGetUniformLocation(this->m_programId, name.c_str()), 1, &value[0]);
		}
		void ShaderProgram::setVec3(const std::string& name, float x, float y, float z) const {
			glUniform3f(glGetUniformLocation(this->m_programId, name.c_str()), x, y, z);
		}

		void ShaderProgram::setVec4(const std::string& name, const glm::vec4& value) const {
			glUniform4fv(glGetUniformLocation(this->m_programId, name.c_str()), 1, &value[0]);
		}
		void ShaderProgram::setVec4(const std::string& name, float x, float y, float z, float w) const {
			glUniform4f(glGetUniformLocation(this->m_programId, name.c_str()), x, y, z, w);
		}

		void ShaderProgram::setMat2(const std::string& name, const glm::mat2& mat) const {
			glUniformMatrix2fv(glGetUniformLocation(this->m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void ShaderProgram::setMat3(const std::string& name, const glm::mat3& mat) const {
			glUniformMatrix3fv(glGetUniformLocation(this->m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		void ShaderProgram::setMat4(const std::string& name, const glm::mat4& mat) const {
			glUniformMatrix4fv(glGetUniformLocation(this->m_programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
		}

		// ======================================
		ShaderProgram* ShaderProgram::createShaderProgram(const std::string& vsResource, const std::string& fsResource) {
			Shader* vsShader = new Shader(GL_VERTEX_SHADER);
			vsShader->createShaderFromFile(vsResource);
			if (vsShader->status() != ShaderStatus::READY) {
				std::cout << "VS: " << vsShader->shaderInfoLog() << "\n";
				return nullptr;
			}

			// fragment shader
			Shader* fsShader = new Shader(GL_FRAGMENT_SHADER);
			fsShader->createShaderFromFile(fsResource);
			if (fsShader->status() != ShaderStatus::READY) {
				std::cout << "FS: " << fsShader->shaderInfoLog() << "\n";
				return nullptr;
			}

			// shader program
			ShaderProgram* shaderProgram = new ShaderProgram();
			shaderProgram->init();
			shaderProgram->attachShader(vsShader);
			shaderProgram->attachShader(fsShader);
			shaderProgram->checkStatus();
			if (shaderProgram->status() != ShaderProgramStatus::READY) {
				return nullptr;
			}
			shaderProgram->linkProgram();

			delete vsShader;
			delete fsShader;

			return shaderProgram;
		}
		ShaderProgram* ShaderProgram::createShaderProgramForComputeShader(const std::string& csResource) {
			// compute shader
			Shader* csShader = new Shader(GL_COMPUTE_SHADER);
			csShader->createShaderFromFile(csResource);
			if (csShader->status() != ShaderStatus::READY) {
				std::cout << "CS: " << csShader->shaderInfoLog() << "\n";
				return nullptr;
			}

			// shader program
			ShaderProgram* shaderProgram = new ShaderProgram();
			shaderProgram->init();
			shaderProgram->attachShader(csShader);
			shaderProgram->checkStatus();
			if (shaderProgram->status() != ShaderProgramStatus::READY) {
				return nullptr;
			}
			shaderProgram->linkProgram();

			delete csShader;

			return shaderProgram;
		}
		ShaderProgram* ShaderProgram::createShaderProgramWithGeometryShader(
			const std::string& vsResource,
			const std::string& fsResource,
			const std::string& gsResource) {

			// Create vertex shader
			Shader* vsShader = new Shader(GL_VERTEX_SHADER);
			vsShader->createShaderFromFile(vsResource);
			if (vsShader->status() != ShaderStatus::READY) {
				std::cout << "VS: " << vsShader->shaderInfoLog() << "\n";
				delete vsShader;
				return nullptr;
			}

			// Create fragment shader
			Shader* fsShader = new Shader(GL_FRAGMENT_SHADER);
			fsShader->createShaderFromFile(fsResource);
			if (fsShader->status() != ShaderStatus::READY) {
				std::cout << "FS: " << fsShader->shaderInfoLog() << "\n";
				delete vsShader;
				delete fsShader;
				return nullptr;
			}

			// Create geometry shader
			Shader* gsShader = new Shader(GL_GEOMETRY_SHADER);
			gsShader->createShaderFromFile(gsResource);
			if (gsShader->status() != ShaderStatus::READY) {
				std::cout << "GS: " << gsShader->shaderInfoLog() << "\n";
				delete vsShader;
				delete fsShader;
				delete gsShader;
				return nullptr;
			}

			// Create shader program
			ShaderProgram* shaderProgram = new ShaderProgram();
			shaderProgram->init();
			shaderProgram->attachShader(vsShader);
			shaderProgram->attachShader(fsShader);
			shaderProgram->attachShader(gsShader);

			// Check program status
			shaderProgram->checkStatus();
			if (shaderProgram->status() != ShaderProgramStatus::READY) {
				std::cout << "Shader program is not ready.\n";
				delete vsShader;
				delete fsShader;
				delete gsShader;
				delete shaderProgram;
				return nullptr;
			}

			// Link the program
			shaderProgram->linkProgram();

			// Clean up shaders (they are no longer needed after linking)
			delete vsShader;
			delete fsShader;
			delete gsShader;

			return shaderProgram;
		}
	}
}

