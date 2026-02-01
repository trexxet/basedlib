#include "Basedlib/LogFile.hpp"

int main () {
	Basedlib::LogFile lf ("basedlib_logfile_test.txt");
	lf.print ("Hello, World! Привет, мир! 你好, 世界! 🚀\n");
	return 0;
}
