#ifndef FILE_DEF_LOADER_H
#define FILE_DEF_LOADER_H
#include "includes.h"
#include "blocks.h"
#include <string>

class FileDefLoader {
public:
	static bool loadDefinitions(std::string file);
};

#endif