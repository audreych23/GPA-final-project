#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices,
	std::vector<unsigned int> indices,
	std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	//std::cout << "======mesh======" << '\n';
	//std::cout << vertices.size() << '\n';
	//std::cout << indices.size() << '\n';
	//std::cout << textures.size() << '\n';
	//if (textures.size() > 0) {
	//	std::cout << "texture path: " << textures[0].path << '\n';
	//}
	//std::cout << "====================" << '\n';

	//setupMesh();

	//this->m_modelMat = glm::mat4(1.0f);
}

//void Mesh::render() {
//	//// WARNING: lazy way of coding (not really but 
//	//// normally should start from zero, but zero is sampler2datrray
//	//glActiveTexture(GL_TEXTURE1);
//	////shader.setInt(name + number, i); done in rendererbhase
//	//glBindTexture(GL_TEXTURE_2D, textures[0].id);
//	//glBindVertexArray(_vao);
//	//glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
//	//glBindVertexArray(0);
//
//	//glActiveTexture(GL_TEXTURE0);
//}
//
//void Mesh::setupMesh() {
//	//glGenVertexArrays(1, &_vao);
//	//glGenBuffers(1, &_vbo);
//	//glGenBuffers(1, &_ebo);
//
//	//glBindVertexArray(_vao);
//	//glBindBuffer(GL_ARRAY_BUFFER, _vbo);
//	//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
//
//	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
//	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
//
//	//// vertex positions for vao
//	//glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::VERTEX_LOCATION);
//	//glVertexAttribPointer(SHADER_PARAMETER_BINDING::VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
//
//	//glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::NORMAL_LOCATION);
//	//glVertexAttribPointer(SHADER_PARAMETER_BINDING::NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//
//	//glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION);
//	//glVertexAttribPointer(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
//
//	//glBindVertexArray(0);
//}