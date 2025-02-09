#include <iostream>
#include <windows.h>
#include <set>
#include <fstream>

#include <direct.h>
#include "filesystem"
#include "vector"

#define red "\033[31m"
#define green "\033[32m"
#define yellow "\033[33m"

#define bold "\033[1m"
#define underline "\033[4m"

#define reset "\033[0m"

#define ok green << "OK!" << reset

using namespace std;

struct stat sb;

set<string> validArgs = {"list","l","ls","unpack","extract","ex","e","x","u","up","pack","archive","ar","a","p","help","h","version","v"};

int total_files;
int fileoffset;
int cpkfilebytessize = 260;
char* cpkfile = new char[cpkfilebytessize];
char* outfilecontent = new char[1073741824];
int cpkfilesize;

bool EnableVirtualTerminalProcessing() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) {
		return false;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode)) {
		return false;
	}
	return true;
}

void help()
{
	cout << "| ================================= |\n";
	cout << "|                                   |\n";
	cout << "|       Crashday Package Tool       |\n";
	cout << "|            by St1ngLeR            |\n";
	cout << "|                                   |\n";
	cout << "| ================================= |\n";
	cout << "\n\nA tool for CPK (Crashday Package) files, which were originally used to install mods for \"classic\" Crashday from 2006.\n";
	cout << bold << "The tool is not made for the Redline Edition!\n\n" << reset;
	cout << bold << underline << "Usage:" << reset << bold << " CD_CPKTool.exe" << reset << " <COMMAND>\n\n";
	cout << bold << underline << "Commands:" << reset;
	cout << "\n  list\t\tList all files in the archive [aliases: l, ls]";
	cout << "\n  unpack\tUnpack / extract files from the archive [aliases: extract, ex, e, x, u, up]";
	cout << "\n  pack\t\tPack / archive files to the archive [aliases: archive, ar, a, p]";
	cout << "\n  help\t\tPrint help information [aliases: h]";
	cout << "\n  version\tPrint version information [aliases: v]\n";
}

void version()
{
	cout << "CPKTool 1.0.0" << endl;
}

void errmsg_args(string subcommand, string arguments[], int length)
{
	cout << red << "ERROR: " << reset << "The following required arguments were not provided:\n";
	for (int i = 0; i <= length; i++)
	{
		cout << green << " " << arguments[i] << "\n";
	}
	cout << reset << bold << underline << "\nUsage:" << reset << bold << " CD_CPKTool.exe " << reset << subcommand;
	for (int i = 0; i <= length; i++)
	{
		cout << " " << arguments[i];
	}
	cout << "\n\nFor more information try '" << bold << "help" << reset << "' or '" << bold << "h" << reset << "'.\n";
}

void errmsg_path(const char* path)
{
	cout << red << "ERROR: " << reset << "The specified file \'" << yellow << path << reset << "' was not found!\n";
	cout << reset "Make sure the file actually exists and provide a valid path to the file.\n";
}

void errmsg_dir(const char* dir)
{
	cout << red << "ERROR: " << reset << "The specified directory \'" << yellow << dir << reset << "' was not found!\n";
	cout << reset "Make sure the directory actually exists and provide a valid path to the directory.\n";
}

void list(const char* target_file)
{
	if (stat(target_file, &sb) == 0 && !(sb.st_mode & S_IFDIR))
	{
		ifstream file(target_file, ios::binary);
		file.seekg(0, ios::beg);
		file.read(reinterpret_cast<char*>(&total_files), 4);

		cout << "Total files: " << total_files << endl;
		cout << setw(10) << left << "\nNum" << setw(40) << " File name" << setw(40) << " File size (in bytes)" << endl;
		for (int i = 0; i < total_files; i++)
		{
			file.seekg(4+(cpkfilebytessize * i) + (12 * i), ios::beg);
			file.read(cpkfile, cpkfilebytessize);

			file.seekg(4 + (cpkfilebytessize * (i+1)) + (12*i)+4, ios::beg);
			file.read(reinterpret_cast<char*>(&cpkfilesize), 4);

			cout << setw(10) << left << i+1 << setw(40) << cpkfile << setw(40) << cpkfilesize << endl;
		}
	}
	else
	{
		errmsg_path(target_file);
	}
}

void unpack(const char* target_file, const char* output_dir = NULL, const char* trkdata_dir = NULL)
{
	string output_dir2;

	if (output_dir == NULL)
	{
		output_dir2 = (string(target_file).substr(0, string(target_file).find_last_of(".")));
	}
	else
	{
		output_dir2 = output_dir;
	}

	string fullpath;

	if (stat(target_file, &sb) == 0 && !(sb.st_mode & S_IFDIR))
	{
		_mkdir(output_dir2.c_str());

		ifstream file(target_file, ios::binary);
		file.seekg(0, ios::beg);
		file.read(reinterpret_cast<char*>(&total_files), 4);

		for (int i = 0; i < total_files; i++)
		{
			file.seekg(4 + (cpkfilebytessize * i) + (12 * i), ios::beg);
			file.read(cpkfile, cpkfilebytessize);

			filesystem::path filePath(cpkfile);
			filesystem::path dirPath = filePath.parent_path();
			filesystem::path filePart = filePath.filename();
			filesystem::create_directories(output_dir2 + string("\\") + dirPath.string());

			cout << "Creating file '";
			if (trkdata_dir != NULL)
			{
				if (dirPath.empty())
				{
					filesystem::create_directories(output_dir2 + string("\\trkdata\\cars\\" + string(trkdata_dir)));
					fullpath = output_dir2 + string("\\trkdata\\cars\\") + trkdata_dir + "\\" + string(cpkfile);
				}
				else
				{
					fullpath = output_dir2 + string("\\") + string(cpkfile);
				}
			}
			else
			{
				fullpath = output_dir2 + string("\\") + string(cpkfile);
			}

			cout << fullpath;
			ofstream outfile(fullpath, ios::binary);
			cout << "'." << endl;

			cout << "Determining the file size - ";
			file.seekg(4 + (cpkfilebytessize * (i + 1)) + (12 * i)+4, ios::beg);
			file.read(reinterpret_cast<char*>(&cpkfilesize), 4);
			cout << cpkfilesize << " bytes. " << endl;

			cout << "Determining the file offset in CPK - ";
			file.seekg(8 + (cpkfilebytessize * (i + 1)) + (12 * i)+4, ios::beg);
			file.read(reinterpret_cast<char*>(&fileoffset), 4);
			cout << fileoffset << " bytes." << endl;

			cout << "Setting the pointer to the found offset... ";
			file.seekg(fileoffset, ios::beg);
			cout << ok << endl;
			
			cout << "Reading the file contents... ";
			file.read(outfilecontent, cpkfilesize);
			cout << ok << endl;

			cout << "Writing the file contents to the output file... ";
			outfile.write(outfilecontent, cpkfilesize);
			cout << ok << endl;
			cout << endl;
		}
		cout << bold << green << "The file has been successfully unpacked!" << reset;
	}
	else
	{
		errmsg_path(target_file);
	}
}

void pack(const char* target_dir, const char* output_file = NULL)
{
	string output_file2;

	if (output_file == NULL)
	{
		output_file2 = target_dir + (string)".cpk";
	}
	else
	{
		output_file2 = output_file;
	}

	if (stat(target_dir, &sb) == 0)
	{
		cout << "Counting the total number of files... ";
		vector<string> filePaths;
		int i = 0;
		for (const auto& entry : filesystem::recursive_directory_iterator(target_dir))
		{
			if (filesystem::is_regular_file(entry))
			{
				i++;
				string relativePath = filesystem::relative(entry.path(), target_dir).string();
				filePaths.push_back(relativePath);
			}
		}
		cout << ok << endl;
		cout << "Creating the CPK file... ";
		filesystem::path filePathOutput(output_file2);
		filesystem::path dirPath = filePathOutput.parent_path();
		filesystem::path filePart = filePathOutput.filename();
		if (!dirPath.empty())
		{
			filesystem::create_directories(dirPath);
		}
		cout << ok << endl;

		ofstream cpkfile(output_file2, ios::binary);
		cpkfile.write(reinterpret_cast<char*>(&i), 4);
		
		cout << "Entering a list of files... ";
		for (const auto& filePath : filePaths)
		{
			cpkfile.write(filePath.c_str(), filePath.length());
			cpkfile.seekp(cpkfilebytessize + 12 - filePath.length(), ios::cur);
		}
		cout << ok << endl;
		
		cout << "Entering a files informations... ";
		cpkfile.seekp(4 + cpkfilebytessize, ios::beg);
		int size = 0;
		int fileoffset = 4 + ((cpkfilebytessize + 12) * i);
		for (const auto& filePath : filePaths)
		{
			int dir;
			if (filePath.find_first_of("\\") != string::npos)
			{
				dir = 1;
			}
			else
			{
				dir = 0;
			}
			cpkfile.write(reinterpret_cast<char*>(&dir), 4);
			fileoffset += size;
			size = filesystem::file_size(target_dir + string("\\") + filePath);
			cpkfile.write(reinterpret_cast<char*>(&size), 4);
			cpkfile.write(reinterpret_cast<char*>(&fileoffset), 4);
			cpkfile.seekp(cpkfilebytessize, ios::cur);
		}
		cout << ok << endl;

		cpkfile.seekp(4 + ((cpkfilebytessize + 12) * i), ios::beg);
		for (const auto& filePath : filePaths)
		{
			cout << "Adding file '" << filePath << "'... ";
			size = filesystem::file_size(target_dir + string("\\") + filePath);
			ifstream file(target_dir + string("\\") + filePath, ios::binary);
			char buffer[1];
			while (file.good())
			{
				file.read(buffer, 1);
				cpkfile.write(buffer, file.gcount());
			}
			
			cout << ok << endl;
		}
		cout << endl;
		cout << bold << green << "The file has been successfully packed!" << reset;
	}
	else
	{
		errmsg_dir(target_dir);
	}
}

int main(int argc, char** argv)
{
	if(EnableVirtualTerminalProcessing())
	{
		for (int i = 1; i < argc; i++) {
			if (argv[i] == "help"sv || argv[i] == "h"sv)
			{
				help();
			}
			else if (argv[i] == "version"sv || argv[i] == "v"sv)
			{
				version();
			}
			else if (argv[i] == "list"sv || argv[i] == "l"sv || argv[i] == "ls"sv)
			{
				if (i + 1 >= argc)
				{
					string arglist[] = { "<CPK_NAME>" };
					errmsg_args("list", arglist, 0);
				}
				else
				{
					list(argv[i + 1]);
				}
			}
			else if (argv[i] == "unpack"sv || argv[i] == "extract"sv || argv[i] == "ex"sv || argv[i] == "e"sv || argv[i] == "x"sv || argv[i] == "u"sv || argv[i] == "up"sv)
			{
				if (i + 1 >= argc)
				{
					string arglist[] = { "<CPK_NAME>", "[OUTPUT_DIR]", "[TRKDATA_DIR]" };
					errmsg_args("unpack", arglist, 2);
				}

				if ((argv[i + 3] == NULL) || (argv[i + 2] == NULL))
				{
					unpack(argv[i + 1], argv[i + 2]);
				}
				else
				{
					unpack(argv[i + 1], argv[i + 2], argv[i + 3]);
				}
			}
			else if (argv[i] == "pack"sv || argv[i] == "archive"sv || argv[i] == "ar"sv || argv[i] == "a"sv || argv[i] == "p"sv)
			{
				if (i + 1 >= argc)
				{
					string arglist[] = { "<INPUT_DIR>", "[CPK_NAME]" };
					errmsg_args("pack", arglist, 1);
				}
				else
				{
					pack(argv[i + 1], argv[i + 2]);
				}
			}
			else if (validArgs.find(argv[1]) == validArgs.end())
			{
				cout << red << "ERROR: " << reset << "The subcommand '" << yellow << argv[i] << reset << "' wasn't recognized!\n\n";
				cout << bold << underline << "Usage:" << reset << bold << " CD_CPKTool.exe" << reset << " <COMMAND>\n\n";
				cout << "For more information try '" << bold << "help" << reset << "' or '" << bold << "h" << reset << "'.\n";
			}
		}
	}
	return 0;
}