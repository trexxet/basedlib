#pragma once

#include <string>

namespace Basedlib {

struct FileDebug {
	FILE* f = nullptr;

	FileDebug (const char *filename);
	void print (const std::string& str);
	~FileDebug ();
};

}
