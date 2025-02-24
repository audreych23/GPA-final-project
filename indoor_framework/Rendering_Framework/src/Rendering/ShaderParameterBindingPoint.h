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
		const GLint POST_PROCESSING_SUB_PROCESS_LOCATION = 4;
		const GLint SHADING_MODEL_ID_LOCATION = 5;
		const GLint KA_LOCATION = 6;
		const GLint KD_LOCATION = 7;
		const GLint KS_LOCATION = 8;
		const GLint KE_LOCATION = 9;
		const GLint NS_LOCATION = 10;
		const GLint HAS_TEXTURE_LOCATION = 11;

		const GLint POST_PROCESSING = 20;
		const GLint LIGHT_BLOOM_POS = 21;

		const GLint LIGHT_VIEW_MAT_LOCATION = 12;
		const GLint LIGHT_PROJ_MAT_LOCATION = 13;
		const GLint LIGHT_BIAS_MAT_LOCATION = 14;

		const GLint AREA_LIGHT_VERTICES_0 = 31;
		const GLint AREA_LIGHT_VERTICES_1 = 32;
		const GLint AREA_LIGHT_VERTICES_2 = 33;
		const GLint AREA_LIGHT_VERTICES_3 = 34;

		const GLint HAS_DIRECTIONAL_LIGHT = 25;
		const GLint HAS_TOON = 26;
		const GLint HAS_NORMAL = 27;
		const GLint HAS_FXAA = 28;
		const GLint IS_METAL = 29;

		const GLint HAS_AREA_LIGHT = 35;

		const GLint DIRECTIONAL_LIGHT_POS = 30;
		const GLint AREA_LIGHT_POS = 40;
		const GLint AREA_LIGHT_ROT = 41;
		const GLint HAS_POINT_SHADOW = 42;
	}

	namespace SHADER_POST_PARAMETER_BINDING {
		const GLuint VERTEX_LOCATION = 0;
		const GLuint TEX_COORDS_LOCATION = 1;
		

		const GLint POST_PROCESSING_ID_LOCATION = 0;
		const GLint POST_PROCESSING_SUB_ID_LOCATION = 1;
		// bloom
		const GLint HORIZONTAL_FLAG_LOCATION = 2;
		const GLint LIGHT_POS_SCREEN_LOCATION = 3;

	}

	namespace POST_PROCESSING_TYPE {
		const GLint REGULAR_EFFECT = 0;
		const GLint BLOOM_EFFECT = 1;
		const GLint DEFERRED_EFFECT = 2;
		const GLint SHADOW_EFFECT = 3;
		const GLint NON_REALISTIC_PHOTO = 4;
		const GLint VOLUMETRIC_LIGHT = 5;

	}
}