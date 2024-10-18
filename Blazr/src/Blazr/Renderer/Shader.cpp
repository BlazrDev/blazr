#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Shader.h"
#include "stb_image.h"

namespace Blazr {
Shader::Shader(GLuint program, const std::string &vertexPath,
			   const std::string &fragmentPath)
	: m_ShaderProgramID(program), m_sVertexPath(vertexPath),
	  m_sFragmentPath(fragmentPath) {}

GLuint Shader::GetUniformLocation(const std::string &name) {
	auto uniformItr = m_UniformLocationMap.find(name);
	if (uniformItr != m_UniformLocationMap.end()) {
		return uniformItr->second;
	}

	GLuint location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
	if (location == GL_INVALID_INDEX) {
		BLZR_CORE_ERROR("Uniform {0} not found in shader.", name);
		return -1;
	}

	m_UniformLocationMap.emplace(name, location);
	return location;
}

Shader::~Shader() {
	if (m_ShaderProgramID > 0) {
		glDeleteProgram(m_ShaderProgramID);
	}
}

void Shader::SetUniformInt(const std::string &name, int value) {
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniformMat4(const std::string &name, glm::mat4 mat) {
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::Enable() { glUseProgram(m_ShaderProgramID); }

void Shader::Disable() { glUseProgram(0); }

GLuint Shader::GetProgramID() const { return m_ShaderProgramID; }
} // namespace Blazr
