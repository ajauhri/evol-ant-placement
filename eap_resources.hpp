#ifndef EAP_RESOURCES_HPP_INCLUDED
#define EAP_RESOURCES_HPP_INCLUDED
#include<iostream>
#include<vector>
#include<fstream>
#include<streambuf>
#include<string>
#include<map>
#include<algorithm.hpp>
#include<algorithms.hpp>
#include<individual.hpp>

namespace eap
{
	//Go for forward declarations...http://stackoverflow.com/questions/2297567/where-should-include-be-put-in-c

	bool to_bool(std::string);

	double rand01();

    unsigned int rand(void);
    unsigned int rand(unsigned int, unsigned int);

	struct EAPException : public std::exception 
	{
		std::string s;
		EAPException(std::string name) : s(name) { } 
		char const* what() const throw() { return s.c_str(); }
		~EAPException() throw() {}
	};

	struct ParseException : public EAPException
	{
		ParseException(std::string msg) : EAPException("Parse exception: " + msg) {}
	};

	struct FileCreationException : public EAPException
	{
		FileCreationException(std::string msg) : EAPException("File creation exception: " + msg) {}
	};

	struct InvalidStateException : public EAPException
	{
		InvalidStateException(std::string msg) : EAPException("Invalid state exception: " + msg) {}
	};

	const std::string generations_folder = "Generations";
	const std::string iterations_folder = "Iterations";
    const std::string run_directory = "runs/";
    const std::string freespace_directory = "free/";
    const std::string results_directory = "results/";

	typedef std::map<std::string, algorithms> a_map;

	a_map create_algorithms_map();

	int get_algorithm(void);
    int fitness_sort(const individual_ptr&, const individual_ptr&);
    int gain_fitness_sort(const individual_ptr&, const individual_ptr&);
    int coupling_fitness_sort(const individual_ptr&, const individual_ptr&);

	extern std::unique_ptr<algorithm> algo;

    extern std::mt19937 gen;
}

#endif



