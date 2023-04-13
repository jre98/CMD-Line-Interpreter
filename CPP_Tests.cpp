
/**
 * DO NOT MODIFY THIS SOURCE FILE.
 */

//
#include <iostream>
#include <string>
#include <vector>

//
#include "Executor.hpp"
#include "Parser.hpp"
#include "puhp-tests/PuhPTests.h"

//
using std::cout, std::cin, std::endl;
using std::string;
using std::vector;

//
using PuhPTests::Tests, PuhPTests::Test, PuhPTests::OutputCapture;

//	PROTO
Test testBasicCommands();
Test testInvalidProgram();
Test testArgumentParsing();
Test testPartRecombination();


//
int main()
{
	//
	Parser parser;
	
	//
	cout << "Begin tests" << endl;
	
	//
	Tests tests;
	
	//
	vector<std::function<Test()>> functions = {
		testBasicCommands,
		testInvalidProgram,
		testArgumentParsing,
		testPartRecombination
	};
	
	//
	for ( auto f : functions ) {
		tests << f();
	}
	
	cout << endl;
	
	cout << tests.getPassFailLogsAsString() << endl;
	
	//
	tests.writeResultsAsGradescopeJson();
	tests.printTestsSummary();
	
	//
	cout << "Tests complete" << endl;
	
	return 0;
}

//
Test testBasicCommands()
{
	//
	Test t("Basic commands");
	
	//
	t.assertNoException(
		[&t]()
		{
			//
			Executor ex;
			
			// Try executing a simple command, `pwd`
			t.assertEqual(ex.executeCommand(&string("pwd")[0]), 1, 1, "Call on pwd (should return 1)");
		},
		1, "No exceptions while running basic commands"
	);
	
	return t;
}

//
Test testInvalidProgram()
{
	//
	Test t("Invalid Program");
	Parser p;
	
	//
	t.assertNoException(
		[&t, &p]()
		{
			//
			string blank = "";
			vector<string> blank_parts;
			
			//
			t.assertFalse(p.isValidProgram(&string("blablabla")[0]), 1, "blablabla shouldn't be a valid program");
			t.assertFalse(p.isValidProgram(blank), 1, "Blank shouldn't be a valid program");
			t.assertTrue(p.isCommandEmpty(blank_parts), 1, "Detect empty command");
		},
		1, "Checking whether a command is valid, shouldn't throw an exception"
	);
	
	return t;
}

//
Test testArgumentParsing()
{
	//
	Test t("Argument parsing");
	
	//
	Parser p;
	
	//
	t.assertNoException(
		[&t, p]()
		{
			//
			std::map<string, vector<string>> data;
			data["echo"] = vector<string>{"echo"};
			data["echo Hello"] = vector<string>{"echo", "Hello"};
			data["echo \"Hello\""] = vector<string>{"echo", "Hello"};
			data["echo Hello hey!"] = vector<string>{"echo", "Hello", "hey!"};
			data["echo \"Hello hey!\""] = vector<string>{"echo", "Hello hey!"};
			data["echo \"With inner quote \\\" there\""] = vector<string>{"echo", "With inner quote \" there"};
			data["echo \"With inner backslash \\\\ there\""] = vector<string>{"echo", "With inner backslash \\ there"};
			
			//
			for ( auto item : data) {
				
				//
				auto parsed = p.parseCommand(item.first.c_str());
				
				//
				t.assertEqual(parsed, item.second, 1, "Parsed vector should be correct for <<<" + item.first + ">>>");
				
				//
				cout << "Oh hi " << item.first << " ==> " << item.second.size() << endl;
			}
			
		}, 1, "No exceptions while testing argument parsing."
	);
	
	//
	t.assertNoException(
		[&t, p]()
		{
			//
			auto parts = p.parseCommand("weffiuh 3238737 wefiuhfh \"82828 22266\" yo");
			
			//
			OutputCapture capture;
			p.printParts(parts);
			
			//
			t.assertEqual(
				capture.getStdout(),
				"Parts:\n> [0] == weffiuh\n> [1] == 3238737\n> [2] == wefiuhfh\n> [3] == 82828 22266\n> [4] == yo\n",
				1,
				"Checking rendered argument list"
			);
			
			
		}, 1, "No exceptions while testing parsed argument rendering"
	);
	
	// Want an exception if we have an unterminated quote
	t.assertException(
		[&t, p]()
		{
			//
			p.parseCommand("echo \"Hello");
			
		}, 1, "Unterminated quote should throw an exception"
	);
	
	// Want an exception if we end on a backslash
	t.assertException(
		[&t, p]()
		{
			//
			p.parseCommand("echo \\");
			
		}, 1, "Backslash just before EOF should throw an exception"
	);
	
	return t;
}

//
Test testPartRecombination()
{
	//
	Test t("Part recombination");
	Parser p;
	
	//
	t.assertNoException(
		[&t, p]()
		{
			//
			vector<string> parts = {
				"echo",
				"Hey there",
				"yip \\ \" pee",
				"yay"
			};
			string command_expected = "echo \"Hey there\" \"yip \\\\ \\\" pee\" \"yay\"";
			string command = p.recombineParts(parts);
			
			//
			t.assertEqual(command, command_expected, 1, "Checking rendered argument list");
			
		}, 1, "No exceptions while testing part recombination"
	);
	
	return t;
}






