#include "blzrpch.h"
#include "Blazr/Core/Log.h"
#include "Shader.h"
#include "stb_image.h"

Blazr::Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // opening the files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;

    // reading files into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure e) {
    BLZR_CORE_ERROR("Error while reading shader file: {0}", e.what());
  }

  const char *vShaderCode = vertexCode.c_str();
  const char *fShaderCode = fragmentCode.c_str();

  // compile the shaders
  unsigned int vShader, fShader;
  int success;
  char infoLog[512];

  vShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShader, 1, &vShaderCode, NULL);
  glCompileShader(vShader);
  glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vShader, 512, NULL, infoLog);
    BLZR_CORE_ERROR("Error while compiling vertex shader: {0}", infoLog);
  }

  fShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShader, 1, &fShaderCode, NULL);
  glCompileShader(fShader);
  glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fShader, 512, NULL, infoLog);
    BLZR_CORE_ERROR("Error while compiling fragment shader: {0}", infoLog);
  }

  ID = glCreateProgram();
  glAttachShader(ID, vShader);
  glAttachShader(ID, fShader);
  glLinkProgram(ID);

  glGetProgramiv(ID, GL_LINK_STATUS, &success);

  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    BLZR_CORE_ERROR("Error while creating shader: {0}", infoLog);
  }

  glDeleteShader(vShader);
  glDeleteShader(fShader);
}

void Blazr::Shader::use() { glUseProgram(ID); }

void Blazr::Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Blazr::Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Blazr::Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
