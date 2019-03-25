#include <iostream>
#include <fstream>

using namespace std;

int main (int argc, char* argv[]) {
	if (argc != 2) {
		printf("Supply file name");
		return 1;
	}

	ofstream primeFile;

	primeFile.open(argv[1], ios::trunc | ios::binary);

	char* data = new char[16] {
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x02,

		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x03
	};

	primeFile.write(data, 16);

	primeFile.close();

	return 0;
}
