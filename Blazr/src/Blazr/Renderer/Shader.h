#pragma once
#include "Blazr/Core/Core.h"
#include <GL/glew.h>
#include <glm.hpp>
#include <string>
#include <unordered_map>

namespace Blazr {
class Shader {
  public:
	Shader(GLuint program, const std::string &vertexPath,
		   const std::string &fragmentPath);
	~Shader();

	void SetUniformInt(const std::string &name, int value);

	void SetUniformMat4(const std::string &name, glm::mat4 mat);

	GLuint GetProgramID() const;

	std::string GetVertexPath() { return m_sVertexPath; }
	std::string GetFragmentPath() { return m_sFragmentPath; }

	int GetUniformInt(const std::string &name) const;
	glm::mat4 GetUniformMat4(const std::string &name) const;

	// use/activate shader
	void Enable();
	void Disable();

  private:
	GLuint m_ShaderProgramID;
	std::string m_sVertexPath, m_sFragmentPath;

	std::unordered_map<std::string, GLuint> m_UniformLocationMap;

	GLuint GetUniformLocation(const std::string &uniformName);

	static Ref<Shader> Create(GLuint program, const std::string &vertexPath,
							  const std::string &fragmentPath);
};
} // namespace Blazr
