#include "FileDialog.h"

#if defined(BLZR_PLATFORM_WINDOWS)
#include <commdlg.h>
#include <windows.h>
#elif defined(BLZR_PLATFORM_LINUX)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#else
#error "Unsupported platform!"
#endif

namespace Blazr {

#if defined(BLZR_PLATFORM_WINDOWS)

std::string FileDialog::OpenFile(const char *filter) {
	char fileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn)) {
		return std::string(fileName);
	}
	return ""; // Dialog was canceled
}

std::string FileDialog::SaveFile(const char *filter) {
	char fileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn)) {
		return std::string(fileName);
	}
	return ""; // Dialog was canceled
}

#elif defined(BLZR_PLATFORM_LINUX)

std::string FileDialog::OpenFile(const char *filter) {
	// Note: `filter` is ignored for Zenity-based dialogs.
	char buffer[256];
	FILE *fp = popen("zenity --file-selection", "r");
	if (fp) {
		if (fgets(buffer, sizeof(buffer), fp) != nullptr) {
			pclose(fp);
			buffer[strcspn(buffer, "\n")] = '\0';
			return std::string(buffer);
		}
		pclose(fp);
	}
	return ""; // Dialog was canceled or failed
}

std::string FileDialog::SaveFile(const char *filter) {
	char buffer[256];
	FILE *fp = popen("zenity --file-selection --save --confirm-overwrite", "r");
	if (fp) {
		if (fgets(buffer, sizeof(buffer), fp) != nullptr) {
			pclose(fp);
			buffer[strcspn(buffer, "\n")] = '\0';
			return std::string(buffer);
		}
		pclose(fp);
	}
	return ""; // Dialog was canceled or failed
}

#endif

} // namespace Blazr
