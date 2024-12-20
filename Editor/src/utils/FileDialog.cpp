#include "FileDialog.h"

#if defined(BLZR_PLATFORM_WINDOWS)
#include <windows.h>
#include <commdlg.h>
#include <shlobj.h>
#include <string>
#elif defined(BLZR_PLATFORM_LINUX)
#include <cstdio>
#include <cstdlib>
#include <cstring>
#else
#error "Unsupported platform!"
#endif

namespace Blazr {

#if defined(BLZR_PLATFORM_WINDOWS)
std::wstring ConvertToWideString(const std::string &str) {
	int size_needed =
		MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size_needed);
	return wstr;
}

// Helper to convert wide (UTF-16) strings to narrow (UTF-8)
std::string ConvertToNarrowString(const std::wstring &wstr) {
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr,
										  0, nullptr, nullptr);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size_needed,
						nullptr, nullptr);
	return str;
}

std::string FileDialog::OpenFile(const char *filter) {
	OPENFILENAMEW ofn = {0};
	wchar_t fileName[MAX_PATH] = L"";

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = ConvertToWideString(filter).c_str();
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameW(&ofn)) {
		return ConvertToNarrowString(fileName);
	}
	return ""; // Dialog was canceled or failed
}

std::string FileDialog::SaveFile(const char *filter) {
	OPENFILENAMEW ofn = {0};
	wchar_t fileName[MAX_PATH] = L"";

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = ConvertToWideString(filter).c_str();
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameW(&ofn)) {
		return ConvertToNarrowString(fileName);
	}
	return ""; // Dialog was canceled or failed
}

std::string FileDialog::OpenFileWithPath(const char *filter,
										 const char *initialPath) {
	OPENFILENAMEW ofn = {0};
	wchar_t fileName[MAX_PATH] = L"";

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = ConvertToWideString(filter).c_str();
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = ConvertToWideString(initialPath).c_str();
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameW(&ofn)) {
		return ConvertToNarrowString(fileName);
	}
	return ""; // Dialog was canceled or failed
}

std::string FileDialog::SaveFileWithPath(const char *filter,
										 const char *initialPath) {
	OPENFILENAMEW ofn = {0};
	wchar_t fileName[MAX_PATH] = L"";

	ofn.lStructSize = sizeof(OPENFILENAMEW);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = ConvertToWideString(filter).c_str();
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = ConvertToWideString(initialPath).c_str();
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileNameW(&ofn)) {
		return ConvertToNarrowString(fileName);
	}
	return ""; // Dialog was canceled or failed
}

std::string FileDialog::OpenFolderWithPath(const char *initialPath) {
	BROWSEINFOW bi = {0};
	bi.lpszTitle = L"Select Folder";
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	PIDLIST_ABSOLUTE pidl = SHBrowseForFolderW(&bi);
	if (pidl) {
		wchar_t folderPath[MAX_PATH];
		if (SHGetPathFromIDListW(pidl, folderPath)) {
			CoTaskMemFree(pidl);
			return ConvertToNarrowString(folderPath);
		}
		CoTaskMemFree(pidl);
	}
	return ""; // Dialog was canceled or failed
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
