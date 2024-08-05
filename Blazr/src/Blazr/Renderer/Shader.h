#ifndef SHADER_H
#define SHADER_H
#include <GL/glew.h>
#include <string>

namespace Blazr {
class Shader {
public:
  // program ID
  unsigned int ID;

  // constructor which reads and builds the shader
  Shader(const char *vertexPath, const char *fragmentPath);

  // use/activate shader
  void use();

  // utility uniform functions
  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
};
} // namespace Blazr

#endif
