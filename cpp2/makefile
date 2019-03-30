out/primegenerator: out/primethread.o out/primegenerator.o
	g++ -g -o out/primegenerator -pthread out/primethread.o out/primegenerator.o

out/primethread.o: src/primethread.cpp
	g++ -g -c -o out/primethread.o -pthread src/primethread.cpp

out/primegenerator.o: src/primegenerator.cpp
	g++ -g -c -o out/primegenerator.o -pthread src/primegenerator.cpp
