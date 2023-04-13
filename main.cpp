#include "Executor.hpp"
#include "Parser.hpp"
#include <iostream>
#include <cstring>


int main()
{
	// Display introduction message
	std::cout << "Welcome to my command line interpreter (aka shell)\nWritten by Jacob Re\n\n";
	// String to hold user input that will be converted to a const char*
	std::string temp;
	// String to hold command once it is ready to be passed to system command
	std::string proper_command;
	// Vector to hold parsed commands
	std::vector<std::string> parts;
	// Loop until user inputs "exit" or "quit"
	while(true) {
		std::cout << "Enter next command\n==> ";
		std::getline(std::cin, temp);
		// Convert to a c string to make sure it will be null terminated
		const char* command = temp.c_str();
		// Create instance of parser that we can use
		Parser p;
		// If user enters exit or quit, break from the loop
		if(strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
			break;
		}
		// Otherwise, if the command is invalid and if it is, print out an error
		else if(!p.isValidProgram(command)) {
			std::cout << "Invalid command!" << std::endl;
		}
		else {
			// Otherwise, continue with the shell execution by first parsing the command
			// Parse inputted command and store in parts vector
			parts = p.parseCommand(command);

			// Use the recombineParts function on the parts vector to reconstruct the parts that have
			// just been parsed and store it in the proper_command variable
			std::string proper_command = p.recombineParts(parts);
			// Convert to a cstring using the c_str function
			command = proper_command.c_str();

			// Use the const_char command to cast the const away from the input so that the
			// executeCommand() function can be used correctly
			char * input = const_cast<char *>(command);
			// Create an instance of the executor
			Executor exe;
			// Execute the command
			exe.executeCommand(input);
		}
	}

	return 0;
}
