#include <iostream>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <thread>
#include <math.h>
#include <unistd.h>

using namespace std;

/**
 * Define constants throughout the file
 */
#define THREAD_COUNT 6
#define MEMORY_CACHE_COUNT 100000000
#define BUFFER_COUNT 100000
#define FLUSH_AMOUNT 10000

/**
 * List of primes that are cached in memory
 */
uint64_t* memoryCache = new uint64_t[MEMORY_CACHE_COUNT];

/**
 * The number that the program is currently checking
 */
uint64_t currentPrime = 5;

/**
 * Used to keep track of primes
 */
uint64_t currentPrimeCount = 0;

/**
 * Used to control the threads in their running state
 */
bool* threadStatus = new bool[THREAD_COUNT];
bool* run = new bool[THREAD_COUNT];
bool isPrime = true;

/**
 * Used to not constantly flush file to disk
 */
uint64_t unflushedPrimeCount = 0;

/**
 * Used to control the process of reading a and writing primes from the file
 */
mutex primeFileLock;

void writePrime(fstream* file, uint64_t prime) {
	primeFileLock.lock();

	char* primeData = new char[8] {
		(char)(prime >> 56 & 0xFF),
		(char)(prime >> 48 & 0xFF),
		(char)(prime >> 40 & 0xFF),
		(char)(prime >> 32 & 0xFF),
		(char)(prime >> 24 & 0xFF),
		(char)(prime >> 16 & 0xFF),
		(char)(prime >>  8 & 0xFF),
		(char)(prime       & 0xFF)
	};

	if (currentPrimeCount < MEMORY_CACHE_COUNT) {
		memoryCache[currentPrimeCount] = prime;
	}

	file->write(primeData, 8);

	primeFileLock.unlock();
}

uint64_t getPrime(fstream* file, uint64_t index, bool force) {
	if (index < MEMORY_CACHE_COUNT && memoryCache[index] != 0 && !force) {
		return memoryCache[index];
	}

	char* fileData = new char[8];
	uint64_t prime = 0;

	primeFileLock.lock();

	file->seekg(index * 8);

	file->read(fileData, 8);

	prime = (
		((uint64_t)fileData[0]) << 56 |
		((uint64_t)fileData[1]) << 48 |
		((uint64_t)fileData[2]) << 40 |
		((uint64_t)fileData[3]) << 32 |
		((uint64_t)fileData[4]) << 24 |
		((uint64_t)fileData[5]) << 16 |
		((uint64_t)fileData[6]) <<  8 |
		((uint64_t)fileData[7])
	);
	
	primeFileLock.unlock();

	return prime;
}

/**
 * The function that operates in its own thread to perform the checks to see if a number is prime
 */
void primeGeneratorThread(fstream* file, char id) {
	while (1) {
		while (!run[id]) {}

		for (uint64_t i = id; i < currentPrimeCount; i += THREAD_COUNT) {
			uint64_t primeCheck = getPrime(file, i, false);

			if (primeCheck == 0) {
				cout << "Cannot find prime number " << i << endl;
				cout << "Current count: " << currentPrimeCount << endl;
				continue;
			}

			if (primeCheck > sqrtl((long double)currentPrime)) {
				break;
			}

			if (currentPrime % primeCheck == 0) {
				isPrime = false;
				break;
			}
		}

		run[id] = false;
		threadStatus[id] = true;
	}
}

/**
 * Checks to see if all the threads are done checking a certain number
 */
bool areThreadsDone() {
	bool areDone = true;

	cout << endl;

	for (char i = 0; i < THREAD_COUNT; i++) {
		if (threadStatus[i] == false) {
			areDone = false;
			break;
		}
	}

	return areDone;
}

int main(int argc, char* argv[]) {
	/**
	 * Check to make sure the file is good
	 */
	if (argc != 2) {
		printf("Supply primes file");
		return 1;
	}

	fstream primeFile;

	primeFile.open(argv[1], ios::in | ios::out | ios::binary | ios::app);

	if (!primeFile.good()) {
		printf("Supply primes file");
		return 1;
	}

	primeFile.seekg(0, primeFile.end);
	currentPrimeCount = (uint64_t)primeFile.tellg() / 8;

	for (uint64_t i = 0; i < currentPrimeCount; i++) {
		currentPrime = getPrime(&primeFile, i, true);
		memoryCache[i] = currentPrime;
	}

	thread* threads = new thread[THREAD_COUNT];

	/**
	 * Initialize all the threads
	 */
	for (char i = 0; i < THREAD_COUNT; i++) {
		threads[i] = thread(primeGeneratorThread, &primeFile, i);
	}

	cout << "Starting with highest prime: " << currentPrime << endl;
	cout << "Current prime count: " << currentPrimeCount << endl;

	/**
	 * Control the threads
	 */
	while (1) {
		isPrime = true;

		currentPrime += 2;

		for (char i = 0; i < THREAD_COUNT; i++) {
			threadStatus[i] = false;
		}

		for (char i = 0; i < THREAD_COUNT; i++) {
			run[i] = true;
		}

		/**
		 * Wait until threads are done
		 */
		while (!areThreadsDone()) {}

		if (isPrime) {
			writePrime(&primeFile, currentPrime);
			currentPrimeCount++;
		}

		cout << unflushedPrimeCount << endl;

		if (++unflushedPrimeCount > FLUSH_AMOUNT) {
			primeFile.flush();
			unflushedPrimeCount = 0;
		}
	}
}
