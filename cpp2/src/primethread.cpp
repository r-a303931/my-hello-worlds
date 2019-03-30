#include <iostream>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <thread>
#include <math.h>
#include <unistd.h>

#include "primethread.h"

char* empty = new char[0];

PrimeThreadHandler::PrimeThreadHandler(
	unsigned char id
): thread_id(id) {
	memory_cache = new uint64_t[MEMORY_CACHE_COUNT];

	char* prime_file_location = new char[15];
	sprintf(prime_file_location, "primes-%u", id);

	std::fstream prime_file;
	prime_file.open(
		prime_file_location,
		std::ios::in | std::ios::out | std::ios::binary | std::ios::app
	);

	if (!prime_file.good()) {
		if (id == 0) {
			prime_to_check = 2;
			save_current_prime();
			prime_count = 1;
		} else if (id == 1) {
			prime_to_check = 3;
			save_current_prime();
			prime_count = 1;
		} else {
			prime_file.write(empty, 0);
			prime_file.flush();
			prime_count = 0;
		}
	} else {
		prime_count = get_prime_count();
	}
	unflushed_prime_count = 0;

	load_memory_cache();
}

bool PrimeThreadHandler::is_done() {
	return !running;
}

uint64_t PrimeThreadHandler::get_prime_count() {
	prime_file.seekg(0, prime_file.end);
	uint64_t file_size = prime_file.tellg();
	return file_size / 8;
}

void PrimeThreadHandler::load_memory_cache() {
	for (int i = 0; i < prime_count; i++) {
		memory_cache[i] = get_prime(i, true);
	}
}

uint64_t PrimeThreadHandler::get_prime(uint64_t index, bool force) {
	if (prime_count == 0) {
		return 0;
	}

	if (!force && index < MEMORY_CACHE_COUNT && memory_cache[index] != 0) {
		return memory_cache[index];
	}

	if (prime_count == 0) {
		return 0;
	}

	char* file_input = new char[8];
	uint64_t result = 0;

	prime_file.seekg(index * 8);
	prime_file.read(file_input, 8);

	result = (
		((uint64_t)file_input[0]) << 56 |
		((uint64_t)file_input[1]) << 48 |
		((uint64_t)file_input[2]) << 40 |
		((uint64_t)file_input[3]) << 32 |
		((uint64_t)file_input[4]) << 24 |
		((uint64_t)file_input[5]) << 16 |
		((uint64_t)file_input[6]) <<  8 |
		((uint64_t)file_input[7])
	);

	return result;
}

uint64_t PrimeThreadHandler::get_largest_prime() {
	if (prime_count == 0) {
		return 0;
	}

	return get_prime(prime_count - 1, false);
}

bool PrimeThreadHandler::check_is_prime() {
	return is_prime;
}

void PrimeThreadHandler::calculate_is_prime() {
	while(1) {
		while (!should_run) {}
		should_run = false;
		running = true;

		for (uint64_t i = 0; i < prime_count; i++) {
			uint64_t prime_check = get_prime(i, false);

			if (should_stop) {
				break;
			}

			if (prime_check == 0) {
				std::cout << "Cannot find prime number " << i << " for thread " << thread_id << std::endl;
				std::cout << "Current count: " << prime_count << std::endl;
				continue;
			}

			if (prime_check > sqrtl((long double)prime_to_check)) {
				break;
			}

			if (prime_to_check % prime_check == 0) {
				is_prime = false;
				break;
			}
		}

		running = false;
	}
}

void PrimeThreadHandler::save_current_prime() {
	char* prime = new char[8] {
		(char)(prime_to_check >> 56 & 0xFF),
		(char)(prime_to_check >> 48 & 0xFF),
		(char)(prime_to_check >> 40 & 0xFF),
		(char)(prime_to_check >> 32 & 0xFF),
		(char)(prime_to_check >> 24 & 0xFF),
		(char)(prime_to_check >> 16 & 0xFF),
		(char)(prime_to_check >>  8 & 0xFF),
		(char)(prime_to_check       & 0xFF)
	};

	if (prime_count < MEMORY_CACHE_COUNT) {
		memory_cache[prime_count] = prime_to_check;
	}

	prime_count++;

	prime_file.write(prime, 8);

	if (++unflushed_prime_count > FLUSH_COUNT) {
		prime_file.flush();
		unflushed_prime_count = 0;
	}
}

void PrimeThreadHandler::start(uint64_t target) {
	prime_to_check = target;
	running = false;

	should_stop = false;
	is_prime = true;
	should_run = true;

	if (!prime_thread.joinable()) {
		prime_thread = std::thread(&PrimeThreadHandler::calculate_is_prime, this);
	}
}

void PrimeThreadHandler::stop() {
	should_stop = true;
}
