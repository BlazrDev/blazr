#pragma once

#include <string>

namespace Blazr {

class FileDialog {
  public:
	static std::string OpenFile(const char *filter = "All Files (*.*)\0*.*\0");
	static std::string OpenFileWithPath(const char *filter,
										const char *initialPath);
	static std::string SaveFileWithPath(const char *filter,
										const char *initialPath);

	static std::string SaveFile(const char *filter = "All Files (*.*)\0*.*\0");
	static std::string OpenFolderWithPath(const char *initialPath);
};

} // namespace Blazr
