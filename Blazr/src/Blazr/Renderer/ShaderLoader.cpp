#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "ShaderLoader.h"
#include <fstream>

namespace Blazr {
GLuint ShaderLoader::CreateProgram(const std::string &vertexShader,
								   const std::string &fragmentShader) {
	const GLuint program = glCreateProgram();
	const GLuint vertex = CompileShader(GL_VERTEX_SHADER, vertexShader);
	const GLuint fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	if (vertex == 0 || fragment == 0) {
		BLZR_CORE_ERROR("Error compiling shaders!");
		return 0;
	}

	if (!LinkShaders(program, vertex, fragment)) {
		BLZR_CORE_ERROR("Failed to link shaders!");
		return 0;
	}

	return program;
}

GLuint ShaderLoader::CompileShader(GLuint shaderType,
								   const std::string &filePath) {
	const GLuint shaderId = glCreateShader(shaderType);
	std::ifstream ifs(filePath);
	if (ifs.fail()) {
		BLZR_CORE_ERROR("Shader {0} failed to open!", filePath);
		return 0;
	}

	std::string contents("");
	std::string line;

	while (std::getline(ifs, line)) {
		contents += line + "\n";
	}

	ifs.close();

	const char *contentsPtr = contents.c_str();
	glShaderSource(shaderId, 1, &contentsPtr, nullptr);

	glCompileShader(shaderId);

	if (!CompileSuccess(shaderId)) {
		BLZR_CORE_ERROR("Failed to compile shader {0}", filePath);
		return 0;
	}

	return shaderId;
}

bool ShaderLoader::CompileSuccess(GLuint shader) {
	GLint status;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE) {
		GLint maxLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::string errorLog(maxLength, ' ');
		glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog.data());

		BLZR_CORE_ERROR("Shader compilation failed: {0}",
						std::string(errorLog));
		glDeleteShader(shader);
		return false;
	}
	return true;
}

bool ShaderLoader::IsProgramValid(GLuint program) {
	GLint status;

	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status != GL_TRUE) {
		GLint maxLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::string errorLog(maxLength, ' ');
		glGetProgramInfoLog(program, maxLength, &maxLength, errorLog.data());

		BLZR_CORE_ERROR("Shader linking failed: {0}", std::string(errorLog));
		return false;
	}
	return true;
}

bool ShaderLoader::LinkShaders(GLuint program, GLuint vertexShader,
							   GLuint fragmentShader) {
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	if (!IsProgramValid(program)) {
		glDeleteProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return false;
	}

	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	return true;
}

std::shared_ptr<Shader>
ShaderLoader::Create(const std::string &vertexShaderPath,
					 const std::string &fragmentShaderPath) {
	GLuint program = CreateProgram(vertexShaderPath, fragmentShaderPath);
	if (program) {
		return std::make_shared<Shader>(program, vertexShaderPath,
										fragmentShaderPath);
	}
	return nullptr;
}
} // namespace Blazr
