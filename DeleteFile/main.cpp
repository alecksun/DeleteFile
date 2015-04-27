#include "windows.h"
#include "stdio.h"
#include "vector"
#include "string"

const int PATH_LENGTH = 32768;

void usage () {
	printf("DeleteFile [File1] [File2] ...\n");
}

bool confirm (wchar_t ** files, int count) {
	printf("You are about to delete the following files and folders:\n");
	for (int i = 0; i < count; i ++) {
		wprintf(files[i]);
		printf("\n");
	}
	while(true) {
		printf("Do you confirm? [Yes/No]");
		char answer[8];
		gets_s(answer, 8);
		if(!_stricmp(answer, "Yes")) {
			return true;
		} else if (!_stricmp (answer, "No")) {
			return false;
		}
	}
}

void deleteFile(const wchar_t * file) {
	wchar_t * szCurPath = new wchar_t[PATH_LENGTH];  
    ZeroMemory(szCurPath, PATH_LENGTH);  
    wsprintf(szCurPath, L"%s\\*", file);  
    WIN32_FIND_DATA FindFileData;        
    ZeroMemory(&FindFileData, sizeof(WIN32_FIND_DATA));  
  
    HANDLE hFile = FindFirstFile(szCurPath, &FindFileData);
	
	bool bIsFile = true;
	std::vector<std::wstring> filenames;
    while (true) {      
		if ( hFile == INVALID_HANDLE_VALUE ) {      
			break;
		}
		bIsFile = false;
		if (wcscmp(L".", FindFileData.cFileName) && wcscmp(L"..", FindFileData.cFileName)) {
			filenames.push_back(FindFileData.cFileName);
		}
		if (!FindNextFile(hFile, &FindFileData)) {
			break;
		}
    } 

	// If there are files found, delete them recursivly
	for (int i = 0; i < filenames.size(); i ++) {
		std::wstring filename = std::wstring(file) + L"\\" + filenames[i].c_str();	
		deleteFile(filename.c_str());
	}

	if ( hFile != INVALID_HANDLE_VALUE) {
		FindClose(hFile);
	}

	if(!SetFileAttributes(file, FILE_ATTRIBUTE_NORMAL)) {
		DWORD errorCode = GetLastError();
		printf("failed to set file attributes: %ls, ErrorCode: %d\n\n", file, errorCode);
	}
	if (bIsFile && !DeleteFile(file)) {
		DWORD errorCode = GetLastError();
		printf("failed to delete file: %ls, ErrorCode: %d\n\n", file, errorCode);
	}

	
	if (!bIsFile &&  !RemoveDirectory(file)) {
		DWORD errorCode = GetLastError();
		printf("failed to delete file: %ls, ErrorCode: %d\n\n", file, errorCode);
	}
	delete [] szCurPath;

}

void deleteFiles(wchar_t ** files, int count) {
	for (int i = 0; i < count; i ++) {
		wchar_t fullPath [PATH_LENGTH];
		GetFullPathName(files[i], PATH_LENGTH, fullPath, NULL);
		std::wstring fname = L"\\\\?\\";
		fname += fullPath;
		deleteFile(fname.c_str());
	}
}


int wmain(int argc, wchar_t ** argv) {
	if (argc < 2) {
		usage();
		return -1;
	}

	if (!confirm(argv + 1, argc - 1)) {
		return 0;
	}

	deleteFiles(argv + 1, argc - 1);

}