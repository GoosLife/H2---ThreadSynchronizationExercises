// ThreadSynchronizationExercises.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <mutex> // Used to lock the critical section

// Class that stores the functions called on the counter in exercise 1
class Counter
{
public:
	/// <summary>
	/// Function that increments the value by 2
	/// </summary>
	void IncrementByTwo(std::string threadName)  //
	{
		// Lock the critical section
		std::lock_guard lg(counter_mtx); // The lock_guard object automatically unlocks the critical section when it goes out of scope, i.e. when the function returns or an exception is thrown.
										 // This is used as the equivalent of a try-finally block in C#.

		try
		{
			// Increment the value by 2
			s_counter += 2;

			// Write the value to the console
			std::cout << "Thread " << threadName << " incremented the value by 2. New value: " << s_counter << '\n';
		}
		catch (...)
		{
			// Do error handling...
			return;
		}
	}

	/// <summary>
	/// Function that decrements the value by 1
	/// </summary>
	void DecrementByOne(std::string threadName)
	{
		std::lock_guard lg(counter_mtx);

		try
		{
			// Decrement the value by 1
			s_counter--;

			// Write the value to the console
			std::cout << "Thread " << threadName << " decremented the value by 1. New value: " << s_counter << '\n';
		}
		catch (...)
		{
			// Error handling
			return;
		}
	}

	/// <summary>
	/// Function that returns the value of the counter
	/// </summary>
	int GetCounter() { return s_counter; }

private:
	inline static int s_counter = 0; // Counter value
	std::mutex counter_mtx; // The mutex lock that governs access to the counter resource
};

// Class that stores the functions called to write to the console in exercise 2
class ConsoleWriter
{
public:
		 //<summary>
		 //Function that writes a character to the console
	 	/// </summary>
	 	void WriteCharacter(char characterToWrite)
	 	{
			std::lock_guard lg(character_mtx);

			try
			{
				// Return early if the character to write is the same as the current character, to ensure that the output is as specified in the exercise
				if (s_currentCharacter == characterToWrite)
				{
					return;
				}

				// Set the current character to the one passed as a parameter
				s_currentCharacter = characterToWrite;

				// Increment the character counter
				s_characterCounter += 60;

				// Write the character to the console
				for (int i = 0; i < 60; i++)
				{
					std::cout << s_currentCharacter;
				}

				std::cout << ' ' << s_characterCounter << '\n';
			}
			catch (...)
			{
				// Error handling
				return;
			}
	 	}

private:
	inline static int s_characterCounter = 0; // Counter used to store how many characters has been currently written to the console
	inline static char s_currentCharacter = '#'; // The current character to be written to console
	std::mutex character_mtx; // The mutex lock that governs access to the static character and character counter variables.
};

int main()
{
	/**
	EXERCISE 1
	*/

	// Create counter object in order to call the non-static methods in the class
	Counter c = Counter();

	// Create 2 threads that call the IncrementByTwo and DecrementByOne methods respectively, as long as the counter value is less than 10
	// Because the counter is incremented by threads in parallel, the loops might not terminate as soon as the counter reaches 10 (depending on the thread scheduling)
	std::thread t1([&c] { 
		while (c.GetCounter() < 10)
		{
			c.IncrementByTwo("thread 1"); 
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		});

	std::thread t2([&c]
		{
			while (c.GetCounter() < 10)
			{
				c.DecrementByOne("thread 2");
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});

	// Join the threads to the main thread so that the program doesn't terminate before the threads finish.
	t1.join();
	t2.join();

	// Destroy the threads after they have finished (garbage collection)
	t1.~thread();
	t2.~thread();

	/**
	EXERCISE 2/3
	*/

	// Bool to specify how long the next two threads should run for
	bool writing = true;

	// Create console writer object in order to call the non-static methods in the class
	ConsoleWriter cw = ConsoleWriter();

	// Threads are created with a reference to the console writer object, as well as the writing boolean that controls how long the threads should run
	std::thread t3([&cw, &writing]
		{
			while (writing)
			{
				cw.WriteCharacter('*');
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});
	std::thread t4([&cw, &writing]
		{
			while (writing)
			{
				cw.WriteCharacter('#');
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
		});

	// Sleep for 10 seconds, then tell the other threads to stop.
	std::this_thread::sleep_for(std::chrono::seconds(10));
	writing = false;

	t3.join();
	t4.join();

	t3.~thread();
	t4.~thread();
}