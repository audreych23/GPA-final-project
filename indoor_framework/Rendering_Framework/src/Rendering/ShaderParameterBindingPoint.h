#pragma once

#include <glad/glad.h>

namespace INANOA {
	namespace SHADER_PARAMETER_BINDING {
		const GLuint VERTEX_LOCATION = 0;
		const GLuint NORMAL_LOCATION = 1;
		const GLuint TEX_COORDS_LOCATION = 2;
		const GLuint TANGENTS_LOCATION = 3;
		//const GLuint INSTANCE_POS_LOCATION = 3;
		

		const GLint MODEL_MAT_LOCATION = 0;
		const GLint VIEW_MAT_LOCATION = 1;
		const GLint PROJ_MAT_LOCATION = 2;
		const GLint CAMERA_POS_LOCATION = 3;
		const GLint SHADING_MODEL_ID_LOCATION = 5;
		const GLint KA_LOCATION = 6;
		const GLint KD_LOCATION = 7;
		const GLint KS_LOCATION = 8;
		const GLint KE_LOCATION = 9;
		const GLint NS_LOCATION = 10;
		const GLint HAS_TEXTURE_LOCATION = 11;
	}
}