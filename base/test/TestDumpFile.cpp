#include "DumpFile.h"



int main(int argc, char **argv)
{
	Base::CDumpFile dump;

	dump.Open("test.txt");

	for(int i = 0; i < 5; i++) {
		dump.Write("just test", 9);
	}

	dump.Close();

	return 0;
}


