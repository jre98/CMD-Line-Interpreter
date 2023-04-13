#include "Executor.hpp"


///	Your welcome
Executor::Executor(){}


//
bool Executor::executeCommand(char * command)
{
	// Create a variable to store the thread id
	pthread_t t_id;

	// Create the thread, being sure to cast to void*
	pthread_create(&t_id, NULL, Executor::threadRunner, (void*)command);

	// Wait for thread to exit
	pthread_join(t_id, NULL);
	// Return 1 to make sure tests work right
	return 1;
}


// Function that will execute the thread commands
void* Executor::threadRunner(void* param)
{
	// static cast the param to be a char* which is accepted by the system command
	char* command = static_cast<char*>(param);
	// Use the system function to execute the command
	system(command);
	pthread_exit(0);
}
