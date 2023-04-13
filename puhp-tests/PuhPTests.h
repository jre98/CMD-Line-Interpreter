/**
 * Copyright 2022 Mike Peralta
 * All Rights Reserved, except my students, who may use this in their homework
 * 
 * Better license coming soon.
 */

//
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


//
#include "json.hpp"


//
namespace PuhPTests
{
	//	Being std::to_string into this namespace,
	//	so we can overload it
	using std::to_string;
	template <class T>
	std::string to_string(const std::vector<T>& v) {
		
		std::stringstream ss;
		bool first = true;
		
		ss << "{";
		for (auto item : v) {
			if (first) {
				first = false;
			}
			else{
				ss << ", ";
			}
			ss << "\"" << item << "\"";
		}
		ss << "}";
		
		return ss.str();
	}
	// lol
	std::string to_string(const std::string& s) {
		return s;
	}
	
	//
	enum class LogEntryType
	{
		None,
		Any,
		Info,
		Pass,
		Fail,
		Warning,
		Error
	};
	class LogEntry
	{
		//
		public:
		
			LogEntry(std::string message, LogEntryType type = LogEntryType::Info)
				: message_(message), type_(type)
			{}
			
			LogEntryType getType() const {
				return this->type_;
			}
			
			//
			std::string toString() const {
				
				std::string tString = LogEntry::typeToString(this->type_);
				if ( this->type_ == LogEntryType::Error || this->type_ == LogEntryType::Fail ) {
					tString = "***" + tString + "***";
				}
				
				return "[" + tString + "] " + this->message_;
			}
			
			static std::string typeToString(LogEntryType t) {
				switch(t)
				{
					case LogEntryType::None:
						return "None";
					case LogEntryType::Any:
						return "Any";
					case LogEntryType::Info:
						return "Info";
					case LogEntryType::Pass:
						return "Pass";
					case LogEntryType::Fail:
						return "Fail";
					case LogEntryType::Warning:
						return "Warning";
					case LogEntryType::Error:
						return "Error";
				}
				throw std::logic_error("Somehow made it to the end of the switch in PuhPTests::LogEntry::typeToString");
			}
			
		private:
			std::string message_;
			LogEntryType type_;
	};
	
	//
	class OutputCapture
	{
		//
		public:
			
			//
			OutputCapture(bool defer_capture = false)
			{
				if ( !defer_capture ) {
					this->beginCapture();
				}
			}
			
			~OutputCapture()
			{
				this->endCapture();
			}
			
			//
			std::string getStdout()
			{
				this->flush();
				return this->stdout_buffer.str();
			}
			std::string getStderr()
			{
				this->flush();
				return this->stderr_buffer.str();
			}
			
			//
			void beginCapture()
			{
				this->endCapture();
				
				this->stdout_buffer.str("");
				this->old_stdout = std::cout.rdbuf(this->stdout_buffer.rdbuf());
				std::cout.rdbuf(this->stdout_buffer.rdbuf());
				
				this->stderr_buffer.str("");
				this->old_stderr = std::cerr.rdbuf(this->stderr_buffer.rdbuf());
				std::cerr.rdbuf(this->stderr_buffer.rdbuf());
			}
			void endCapture()
			{
				//
				this->flush();
				
				if ( this->old_stdout != nullptr ) {
					std::cout.rdbuf(this->old_stdout);
					this->old_stdout = nullptr;
				}
				if ( this->old_stderr != nullptr ) {
					std::cerr.rdbuf(this->old_stderr);
					this->old_stderr = nullptr;
				}
			}
			
			void flush()
			{
				std::cout << std::flush;
				std::cerr << std::flush;
			}
			
		private:
			
			//
			std::stringstream stdout_buffer;
			std::stringstream stderr_buffer;
			
			//
			std::streambuf* old_stdout = nullptr;
			std::streambuf* old_stderr = nullptr;
	};
	
	//
	class Test
	{
		public:
			
			//
			static constexpr const char * DEFAULT_LABEL = "NOLABEL";
			
			//
			Test(std::string label, int points = 0, int points_possible = 0)
				: label_(label), points_(points), points_possible_(points_possible)
			{
				this->log("Instance created");
			}
			Test(const Test& other) {
				this->copyOther(other);
			}
			
			//
			std::string getLabel() const { return this->label_; }
			
			//
			void assertTrue(bool b, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				this->adjustPointsPossible(points, label);
				
				if ( b == true ) {
					this->adjustPoints(points, label + " :: Assertion is true");
				}
				else {
					this->log(
						LogEntry(
							label + " :: Unable to award " + std::to_string(points) + " points; Assertion was not true",
							LogEntryType::Fail
						)
					);
				}
			}
			
			//
			void assertFalse(bool b, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				this->adjustPointsPossible(points, label);
				
				if ( b == false ) {
					this->adjustPoints(points, label + " :: Assertion is false");
				}
				else {
					this->log(
						LogEntry(
							label + " :: Unable to award " + std::to_string(points) + " points; Assertion was not false",
							LogEntryType::Fail
						)
					);
				}
			}
			
			//
			template<class T1, class T2>
			void assertEqual(T1 a, T2 b, int points = 0, std::string label = Test::DEFAULT_LABEL)
			{
				//
				bool is_equal = a == b;
				std::string comparison_label = (is_equal ? " == " : " != ");
				
				// Use to_string instead of std::to_string so we have access to overloads
				this->assertTrue(is_equal, points, label + " (" + to_string(a) + comparison_label + to_string(b) + ")");
			}
			
			//
			void assertException(std::function<void()> func, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				this->adjustPointsPossible(points, label);
				
				//
				bool caught_exception = false;
				try
				{
					func();
				}
				catch( const std::exception& e )
				{
					this->adjustPoints(points, label + " :: Exception was encountered ==> " + e.what());
					caught_exception = true;
				}
				catch( ... )
				{
					this->adjustPoints(points, label + " :: Exception of unrecognized type was encountered.");
					caught_exception = true;
				}
				
				//
				if ( caught_exception == false ) {
					this->log(
						LogEntry(
							label + " :: Unable to award " + std::to_string(points) + " points; No exception was encountered.",
							LogEntryType::Fail
						)
					);
				}
			}
			
			//
			void assertNoException(std::function<void()> func, int points = 0, std::string label = Test::DEFAULT_LABEL) {
				
				this->adjustPointsPossible(points, label);
				
				//
				bool caught_exception = false;
				try
				{
					func();
				}
				catch( ... )
				{
					caught_exception = true;
				}
				
				//
				if ( caught_exception == false ) {
					this->adjustPoints(points, label + " :: Exception wasn't encountered.");
				}
			}
			
			//
			void log(const LogEntry& entry) {
				this->logs.push_back(entry);
				std::cout << "[" << this->label_ << "]" << entry.toString() << std::endl;
			}
			void log(const std::string& s) {
				LogEntry entry(s, LogEntryType::Info);
				this->log(entry);
			}
			
			//
			Test& operator=(const Test& rhs) {
				this->copyOther(rhs);
				return *this;
			}
			
			//
			int adjustPoints(int adjustment, std::string reason, LogEntryType log_type = LogEntryType::None) {
				
				//
				int points_old = this->points_;
				this->points_ += adjustment;
				
				//
				if ( log_type == LogEntryType::None) {
					if (adjustment > 0) {
						log_type = LogEntryType::Pass;
					}
					else if (adjustment < 0) {
						log_type = LogEntryType::Fail;
					}
				}
				this->log(
					LogEntry(
						"Adjusting points from " + std::to_string(points_old) + " to " + std::to_string(this->points_) + " (" + std::to_string(adjustment) + ") because: " + reason,
						log_type
					)
				);
				
				return this->points_;
			}
			
			//
			int adjustPointsPossible(int points, std::string label) {
				
				//
				if (points > 0) {
					this->points_possible_ += points;
					this->log(label + " :: Adjusting points possible to " + std::to_string(this->points_possible_));
				}
				
				return this->points_possible_;
			}
			
			//
			int getPoints() const { return this->points_; }
			int getPointsPossible() const { return this->points_possible_; }
			
			//
			std::string getLogsAsString(std::vector<LogEntryType> log_types = {LogEntryType::Any}) {
				
				//
				std::stringstream ss;
				
				//
				bool any = std::find(log_types.begin(), log_types.end(), LogEntryType::Any) != log_types.end();
				for (const LogEntry& entry : this->logs) {
					
					LogEntryType t = entry.getType();
					
					if ( any || std::find(log_types.begin(), log_types.end(), t) != log_types.end() ) {
						ss << "[" << this->getLabel() << "]" << entry.toString() << std::endl;
					}
				}
				
				return ss.str();
			}
			std::string getLogsAsString(LogEntryType t) {
				return this->getLogsAsString({t});
			}
			std::string getPassFailLogsAsString() {
				return this->getLogsAsString({LogEntryType::Pass, LogEntryType::Fail});
			}
			
			//
			nlohmann::json getLogsAsJson() {
				
				//
				nlohmann::json j;
				
				//
				for (const LogEntry& entry : this->logs) {
					j.push_back(entry.toString());
				}
				
				return j;
			}
			
			//
			nlohmann::json toJson() {
				return nlohmann::json {
					{"label", this->label_},
					{"logs", this->getLogsAsString() },
					{"points", this->points_},
					{"points_possible", this->points_possible_}
				};
			}
			
			//
			nlohmann::json toGradescopeJson() {
				return nlohmann::json {
					{"score", this->points_},
					{"max_score", this->points_possible_},
					{"output", this->getLogsAsString()},
					{"name", this->label_}
				};
			}
			
		private:
			
			//
			std::string label_;
			int points_;
			int points_possible_;
			std::vector<LogEntry> logs;
			
			//
			void copyOther(const Test& other) {
				this->label_ = other.label_;
				this->points_ = other.points_;
				this->points_possible_ = other.points_possible_;
				this->logs = other.logs;
			}
	};
	
	//
	class Tests
	{
		public:
			
			//
			static constexpr int DEFAULT_INDENT = 3;
			static constexpr const char * DEFAULT_RESULTS_FILENAME = "results.json";
			
			// CTOR
			Tests() {}
			
			//
			void add(const Test& test) {
				this->tests_.push_back(test);
			}
			
			//
			nlohmann::json getTestsAsJson() const {
				
				//
				nlohmann::json j;
				for ( Test tt : this->tests_) {
					j.push_back(tt.toJson());
				}
				
				return j;
			}
			nlohmann::json getTestsAsGradescopeJson() const {
				
				//
				nlohmann::json j;
				for ( Test tt : this->tests_) {
					j.push_back(tt.toGradescopeJson());
				}
				
				return j;
			}
			
			//
			nlohmann::json getResultsAsJson() const {
				
				//
				nlohmann::json results;
				
				//
				results["output"] = this->getTestsSummaryAsString();
				results["tests"] = this->getTestsAsJson();
				
				return results;
			}
			nlohmann::json getResultsAsGradescopeJson() const {
				
				//
				nlohmann::json results;
				
				//
				results["output"] = this->getTestsSummaryAsString();
				results["tests"] = this->getTestsAsGradescopeJson();
				
				return results;
			}
			
			//
			std::string getResultsAsString() {
				
				//
				nlohmann::json results;
				
				//
				results["tests"] = this->getTestsAsJson();
				
				return results.dump(Tests::DEFAULT_INDENT);
			}
			
			//
			void writeResultsAsJson(std::string file_path = Tests::DEFAULT_RESULTS_FILENAME ) const {
				
				//
				nlohmann::json results = this->getResultsAsJson();
				
				//
				{
					std::ofstream f(file_path);
					f << results.dump(Tests::DEFAULT_INDENT);
				}
			}
			void writeResultsAsGradescopeJson(std::string file_path = Tests::DEFAULT_RESULTS_FILENAME ) const {
				
				//
				nlohmann::json results = this->getResultsAsGradescopeJson();
				
				//
				{
					std::ofstream f(file_path);
					f << results.dump(Tests::DEFAULT_INDENT);
				}
			}
			
			//
			void printTestsSummary() {
				std::cout << this->getTestsSummaryAsString() << std::endl;
			}
			
			//
			std::string getTestsSummaryAsString() const {
				
				//
				std::stringstream ss;
				
				//
				ss << "***** Tests Summary *****" << std::endl;
				
				//
				int total = 0, total_possible = 0;
				for ( const Test& t : this->tests_ ) {
					
					//
					total += t.getPoints();
					total_possible += t.getPointsPossible();
					
					//
					ss << "[" << t.getLabel() << "] " << t.getPoints() << " / " << t.getPointsPossible() << std::endl;
				}
				
				//
				ss << std::endl;
				ss << "Total: " << total << " / " << total_possible << std::endl;
				
				return ss.str();
			}
			
			//
			std::string getPassFailLogsAsString() const {
				
				std::stringstream ss;
				
				ss << "***** Pass / Fail Logs *****" << std::endl;
				
				for ( auto t : this->tests_ ) {
					ss << t.getPassFailLogsAsString();
				}
				return ss.str();
			}
			
			//	Overload the stream operator to do appending
			Tests& operator<<(const Test& t) {
				
				this->add(t);
				
				return *this;
			}
			
		private:
			
			std::vector<Test> tests_;
	};
}




