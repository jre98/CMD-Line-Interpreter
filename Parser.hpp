#ifndef PARSER_HPP
#define PARSER_HPP

/**
 * You do not really need to edit this file.
 * You may add additional class methods to Parser, if you wish.
 * But do not modify or remove the existing class methods.
 */

//
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

//	Class you can throw in case of error
class InvalidCommandError
{
	public:
		InvalidCommandError(std::string what) : what_(what) {}
		std::string what() const { return this->what_; }
	private:
		std::string what_;
};

//
class Parser
{
	public:
		
		//
		Parser();
		
		//
		static std::vector<std::string> parseCommand(const char * command);
		static void unquotedMode();
		static void quotedMode();
		static std::string recombineParts(const std::vector<std::string>& parts);
		
		static bool isCommandEmpty(const std::vector<std::string>& parts);
		
		static bool isValidProgram(std::string program);
		static bool isValidProgram(const std::vector<std::string>& parts);
		static bool isValidProgram(const char * program);
				
		static void printParts(const std::vector<std::string>& parts);
		
	private:
		
		//
		
};

#endif
