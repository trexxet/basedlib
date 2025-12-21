#pragma once

#include <cstdio>

namespace Basedlib {

struct FileDebug {
	FILE* f = nullptr;

	FileDebug (const char *filename);
	void print (const char *str);
	~FileDebug ();
};

}
