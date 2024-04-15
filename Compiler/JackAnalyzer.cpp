#include "CompilationEngine.h"
// #include "dirent.h"

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dirent.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cerr << "No file provided.";
		return -1;
	}

	std::string argv_path(argv[1]);
	std::string extension = ".jack";

	std::vector<std::string> argv_files;

	// Input is file.
	if (argv_path.find(extension, (argv_path.length() - extension.length())) != std::string::npos)
	{
		argv_files.push_back(argv[1]);
	}
	// Input is directory.
	else {	
		if (argv_path.find_last_of("\\/") != argv_path.length() - 1)
		{
			argv_path.append("/");
		}

		DIR* directory;
		struct dirent* entry;
		if ((directory = opendir(argv[1])) != NULL) {
			while ((entry = readdir(directory)) != NULL) {
				std::string fname = entry->d_name;
				std::string path = argv_path;
				if (fname.find(extension, (fname.length() - extension.length())) != std::string::npos)
				{
					argv_files.push_back(path.append(fname));
				}
			}
			closedir(directory);
		}
	}

	for (std::vector<std::string>::iterator src_file = argv_files.begin(); src_file != argv_files.end(); ++src_file)
	{
		/** Setup file name and file path */
		std::string target_name;
		std::string target_path = *src_file;

		bool is_path = true;

		const size_t index_extension = target_path.find_last_of(".jack");
		if (index_extension == target_path.length() - 1)
		{
			target_path.erase(index_extension - 4);
			is_path = false;
		}

		const size_t index_last_slash = target_path.find_last_of("\\/");
		if (index_last_slash == target_path.length() - 1)
		{
			target_path.erase(index_last_slash);
		}

		target_name = target_path.substr(target_path.find_last_of("\\/") + 1);

		if (is_path) {
			target_path.erase(index_extension - 4);
			target_path.append("/").append(target_name).append(".xml");
		}
		else {
			target_path.append(".xml");
		}

		/** Run syntax analysis */
		CompilationEngine engine(*src_file, target_path);
		engine.CompileClass();
		engine.close();
	}

	return 0;
}
