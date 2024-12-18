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
std::string FileDialog::OpenFolderWithPath(const char *initialPath) {
	BROWSEINFO bi = {0};
	bi.lpszTitle = "Select Folder";
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	// Use the initial path if provided
	if (initialPath) {
		bi.lpfn = [](HWND hwnd, UINT uMsg, LPARAM lParam,
					 LPARAM lpData) -> int {
			if (uMsg == BFFM_INITIALIZED) {
				SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
			}
			return 0;
		};
		bi.lParam = (LPARAM)initialPath;
	}

	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	if (pidl != nullptr) {
		char path[MAX_PATH];
		if (SHGetPathFromIDList(pidl, path)) {
			CoTaskMemFree(pidl); // Free memory
			return std::string(path);
		}
		CoTaskMemFree(pidl); // Free memory even if no path was returned
	}
	return ""; // Dialog was canceled or failed
}

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

std::string FileDialog::OpenFileWithPath(const char *filter,
										 const char *initialPath) {
	char fileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = initialPath;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn)) {
		return std::string(fileName);
	}
	return ""; // Dialog was canceled
}

std::string FileDialog::SaveFileWithPath(const char *filter,
										 const char *initialPath) {
	char fileName[MAX_PATH] = "";

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = initialPath;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn)) {
		return std::string(fileName);
	}
	return ""; // Dialog was canceled
}

#elif defined(BLZR_PLATFORM_LINUX)

std::string FileDialog::OpenFile(const char *filter) {
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

std::string FileDialog::OpenFileWithPath(const char *filter,
										 const char *initialPath) {
	std::string command =
		"zenity --file-selection --filename=" + std::string(initialPath) + "/";
	char buffer[256];
	FILE *fp = popen(command.c_str(), "r");
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

std::string FileDialog::SaveFileWithPath(const char *filter,
										 const char *initialPath) {
	std::string command =
		"zenity --file-selection --save --confirm-overwrite --filename=" +
		std::string(initialPath) + "/";
	char buffer[256];
	FILE *fp = popen(command.c_str(), "r");
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

std::string FileDialog::OpenFolderWithPath(const char *initialPath) {
	std::string command = "zenity --file-selection --directory";
	if (initialPath) {
		command += " --filename=" + std::string(initialPath) + "/";
	}

	char buffer[256];
	FILE *fp = popen(command.c_str(), "r");
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
