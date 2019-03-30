#include "primethread.h"
#include <iostream>
#include <stdint.h>
#include <vector>

#define THREAD_COUNT 6

int main(int argc, char* argv[]) {
	std::vector<PrimeThreadHandler> prime_threads;

	uint64_t current_prime = 0, largest_prime_for_thread, prime_count = 0;

	for (unsigned char i = 0; i < THREAD_COUNT; i++) {
		prime_threads.push_back(PrimeThreadHandler(i));

		largest_prime_for_thread = prime_threads[i].get_largest_prime();

		current_prime = largest_prime_for_thread > current_prime ? largest_prime_for_thread : current_prime;

		prime_count += prime_threads[i].get_prime_count();
	}

	int current_prime_saver = 0;

	while (true) {
		current_prime += 2;

		for (int i = 0; i < THREAD_COUNT; i++) {
			prime_threads[i].start(current_prime);
		}

		bool done = false;
		do {
			for (int i = 0; i < THREAD_COUNT; i++) {
				done = done || prime_threads[i].is_done();
			}
		} while (!done);

		for (int i = 0; i < THREAD_COUNT; i++) {
			prime_threads[i].stop();
		}

		prime_threads[current_prime_saver].save_current_prime();

		current_prime_saver++;
		current_prime_saver %= THREAD_COUNT;
	}

	return 0;
}
