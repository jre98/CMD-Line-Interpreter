#include "Parser.hpp"

/// Your welcome
Parser::Parser() {}

/**
 * The parseCommand method received a constant character pointer
 * representing a raw command line, and parses it into separate parts.
 * Each part represents one argument. For example, "ping -c 5 192.168.1.1"
 * would result in the following list of parts (aka arguments)
 * Arg[0] == ping
 * Arg[1] == -c
 * Arg[2] == 5
 * Arg[3] == 192.168.1.1
 *
 * The function will return the generated vector of arguments to the caller.
 */


std::vector<std::string> Parser::parseCommand(const char * command)
{
 // Bool to determine if we are in quoted or unquoted mode, set to unquoted as we start in unquoted
	bool is_quoted = false;
	// Vector to hold the parsed command. This will be returned from the function and each entry in
	// the vector will be one part of the command
	std::vector<std::string> args;
	// Variable to hold the command that is currently being parsed
	std::string curr_command = "";

	// Store length of command plus 1, as strlen does not count the null terminator in its length
	unsigned int len = std::string(command).length() + 1;


	// Begin the code in quoted mode and have a loop to go through each char in the input
	for(unsigned int i = 0; i < len; i++) {
		// // Boolean used to control command flow and exceptions
		// bool backslash_error = false;
		if(!is_quoted) {
			// Case where we have a null terminator, in which case the command would be valid
			if(command[i] == '\0') {
                args.push_back(curr_command);
				// std::cout << "The command was parsed successfully" << std::endl;
			}
			// If we have a space, we push back the command vector with the current command and
			// also reset the curr_command
			else if(command[i] == ' ') {
				args.push_back(curr_command);
				curr_command = "";
			}
			// If we have a backslash, we know the next character will be literal and we will append
			// curr_command with the next literal character. We catch the case where a \ is the last
			// character in a later if/else statement
			else if(command[i] == '\\' && command[i + 1] != '\0') {
				curr_command += command[i + 1];
			}
			// Case where the last character in the command is a '\'
			else if(command[i] == '\\' && command[i + 1] == '\0') {
				throw InvalidCommandError("Invalid command, last character is a \\");
			}
			// If we encounter a quote, we enter quoted mode
			else if(command[i] == '"') {
				is_quoted = true;
			}
			// In all other cases, we just append the current character and move onto the next
			else {
				curr_command += command[i];
			}
		}

		else if(is_quoted) {
            // If we encounter another quote, we have found the end of the quote, so we will
            // leave quoted mode
      if(command[i] == '"') {
          is_quoted = false;
      }

			// If we encounter a backslash, there are two possibilities
			else if(command[i] == '\\') {
	      // If there are no characters after this (ie, if we have reached the end of the
	      // string), we throw an error
	      if(command[i + 1] == '\0') {
					throw InvalidCommandError("Encountered backslash just before EOF.");
	      }
	      // Otherwise, append the next character, as we use the \ as a literal
	      else {
	          curr_command += command[i + 1];
						i++;
	      }
      }
            // In the case that we encounter a null terminator in quoted mode, we throw an error
      else if(command[i] == '\0') {
				throw InvalidCommandError("Encountered null terminator in quoted mode.");
      }

            // The other possibility is that we just have a character, any of which are acceptable,
            // // so we append that character to the current command
      else {
          curr_command += command[i];
      }
		}
	}
	// Print the arguments that have been parsed
	printParts(args);
	return args;

}


/**
 * The recombineParts method receives a constant reference to a vector-of-strings,
 * and recombines them into a command that system() can use.
 * The idea here is fairly easy:
 * - Start with an empty string.
 * - Iterate over each part.
 * - For each part, iterate over each character it contains.
 * - For each character, append it to the new string,
 *     making sure to add a backslash before any quote or backslash characters.
 * - Separate each part with a space
 *
 * You'll also want to quote each argument but the first.
 *
 * Example:
 * {"ping", "-c", "1", "192.168.1.1"} becomes: ping "-c" "1" "192.168.1.1"
 *
 * The function will return the string to the caller.
 */
std::string Parser::recombineParts(const std::vector<std::string>& parts)
{
	// String to hold user command
	std::string user_command = "";
	// Since we do not quote the first argument, it was just easier to perform that operation
	// outside of the for loops
	// Loop through each character in the first entry in parts vector
	for(unsigned int i = 0; i < parts[0].length(); i++) {
		// If we encounter a backslash or a quote, add a backslash before that character
		if(parts[0][i] == char('\\') || parts[0][i] == char('"')) {
			user_command += "\\";
			user_command += parts[0][i];
		}
		// Otherwise, just append the user command with the character
		else {
			user_command += parts[0][i];
		}
	}


	// Now that we have finished the first argument, use nested loops to loop through each character
	// in each string in the vector
	for(unsigned int i = 1; i < parts.size(); i++) {
		// Add a space and quote before each argument besides the first
		user_command += " ";
		user_command += "\"";
		for(unsigned int j = 0; j < parts[i].length(); j++) {
			// Same deal as above when encountering a backslash or a quote
			if(parts[i][j] == char('\\') || parts[i][j] == char('"')) {
				user_command += "\\";
				user_command += parts[i][j];
			}
			else {
				user_command += parts[i][j];
			}
		}
		user_command += "\"";
	}
	return user_command;
}

/**
 * The isCommandEmpty takes a vector of parsed command parts
 * and decides if it is empty or not.
 * Return true is the incoming vector is empty, false otherwise
 */
bool Parser::isCommandEmpty(const std::vector<std::string>& parts)
{
	// If the vector's size is 0, it is empty so return true
	if(parts.size() == 0) {
		return true;
	}
	// Otherwise, return false as the vector is not empty
	else {
		return false;
	}
}

/**
 * The isValidProgram function takes in a string representing the name or path
 * to a program to be executed.
 * If the program is in our list of valid/acceptable programs, it returns true
 * Otherwise, it returns false
 */

// For now, I assume that just the first part determines if the command is valid or not
bool Parser::isValidProgram(std::string program)
{
	// List of valid commands given assumption above
	std::string valid_commands[7] = {"ls", "df", "pwd", "ps", "gedit", "echo", "ping"};
	// Temporary string to hold first 'word' in program command
	std::string temp;
	// Loop through until we find a space which inidcates we have reached the end of the argument
	for(unsigned int i = 0; i < program.length(); i++) {
		if(program[i] == ' ') {
			break;
		}
		else {
			temp += program[i];
		}
	}
	// Loop through the valid commands and determine if the command the user entered is in the list
	for(int i = 0; i < 7; i++) {
		// If we find the value, return true
		if(temp == valid_commands[i]) {
			return true;
		}
	}
	// Otherwise, return false
	return false;
}

/// Your welcome
bool Parser::isValidProgram(const std::vector<std::string>& parts)
{
	if ( !parts.empty() ) {
		return Parser::isValidProgram(parts[0]);
	}

	return false;
}

/// Your welcome
bool Parser::isValidProgram(const char * program) { return Parser::isValidProgram(std::string(program)); }

/**
 * The printParts function simply prints out a list of command line parts (aka arguments)
 * First, print "Parts:" on a line.
 * Next, loop over each part and print "> [a] == b",
 *   where 'a' represents the part's index,
 *   and 'b' represents the part's value.
 */
void Parser::printParts(const std::vector<std::string>& parts)
{
	std::cout << "Parts:\n";
	// Print out the list of the parts of the entered command
	for(unsigned int i = 0; i < parts.size(); i++) {
		std::cout << "> [" << i << "] == " << parts[i] << std::endl;
	}
}
