#ifndef PRIMETHREAD_H_
#define PRIMETHREAD_H_ 

#include <iostream>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <thread>
#include <math.h>
#include <unistd.h>

#define MEMORY_CACHE_COUNT 1048576
#define FLUSH_COUNT 65536

class PrimeThreadHandler {
	public:
		/**
		 * Initializes the class responsible for handling a thread
		 */
		PrimeThreadHandler(unsigned char id);
		/**
		 * Starts running a thread to calculate if the target is not a prime number given the primes this thread has access to
		 */
		void start(uint64_t target);
		/**
		 * Stops the currently running thread
		 */
		void stop();
		/**
		 * Checks to see if thread is done running
		 */
		bool is_done();
		/**
		 * Checks to see if the result of the thread is a number that is prime
		 */
		bool check_is_prime();
		/**
		 * Each handler has its own file, and will be told by the main thread if it needs to save the to its file
		 *
		 * The main thread will stagger out which thread saves a file; e.g., thread 1 will save the first prime, thread 2 will save the second prime, etc
		 */
		void save_current_prime();
		/**
		 * A function to get how many primes this thread has, checks how big the prime file for this thread is
		 */
		uint64_t get_prime_count();
		/**
		 * Gets the largets prime this thread has in its file
		 */
		uint64_t get_largest_prime();

	private:
		/**
		 * Used to keep track of this thread
		 */
		unsigned char thread_id;
		/**
		 * A variable to determine if the thread is currently running
		 */
		bool running;
		/**
		 * A signal variable to determine if the thread should run
		 */
		bool should_run;
		/**
		 * Used to represent whether or not the number being checked is currently prime; it is assumed so until proven false
		 */
		bool is_prime;
		/**
		 * A signal variable to determine if the thread should stop
		 */
		bool should_stop;

		/**
		 * The file to store the primes in
		 */
		std::fstream prime_file;

		/**
		 * The current prime being checked
		 */
		uint64_t prime_to_check;
		/**
		 * The amount of primes this thread has access to
		 */
		uint64_t prime_count;
		/**
		 * How many primes this thread has to force a flush for
		 */
		uint64_t unflushed_prime_count;
		/**
		 * The memory cache that is used to skip disk access
		 */
		uint64_t* memory_cache;

		/**
		 * Helper function that will return the prime number located at index
		 */
		uint64_t get_prime(uint64_t index, bool force);

		/**
		 * Function called by `thread' to check if number is prime
		 */
		void calculate_is_prime();

		/**
		 * Used by the class to load the memory cache that is used during runtime
		 */
		void load_memory_cache();

		/**
		 * Used to keep track of the actual thread
		 */
		std::thread prime_thread;
};

#endif
