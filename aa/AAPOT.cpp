// AAPOT.cpp : Defines the entry point for the console application.
//
#include<iostream>
#include<fstream>
#include<time.h>
#include<boost/program_options.hpp>
#include<boost/filesystem.hpp>
#include "selector.hpp"
#include "ga.hpp"
#include "sa.hpp"
#include "es.hpp"
#include "hc.hpp"
#include "algorithms.hpp"
#include "algorithm.hpp"


namespace prg_opts = boost::program_options;
using namespace std;

int main(int argc, char* argv[])
{	
	try 
	{
		std::string aapot_file;
		std::string config_file;
		
		prg_opts::options_description desc("\nUsage");
		desc.add_options()
			("help,h", "view valid options")
			("aapot,A", prg_opts::value<string>(&aapot_file), "AAPOT-xml file name")
			("config,C", prg_opts::value<string>(&config_file), "Config-xml file name");
		
		prg_opts::variables_map vm;
		prg_opts::store(prg_opts::parse_command_line(argc, argv, desc), vm);
		prg_opts::notify(vm);

		if (vm.count("help") || !vm.count("aapot") || !vm.count("config") )
		{
				std::cout << desc << "\n";
				return 0;
		}
		
		if (!std::ifstream(aapot_file.c_str()) || !std::ifstream(config_file.c_str()))
		{
			std::cout << "Can not open file input files" << "\n";
			return 0;
		}

		selector *algo_config = new selector(aapot_file);
		algorithm *algo;

		switch(algo_config->get_algorithm())
		{
		case GA:
			std::cout<<"Testing GA"<<std::endl;
			algo = new ga(aapot_file, config_file);
			break;
		case SA:
			std::cout<<"Testing SA"<<std::endl;
			algo = new sa(aapot_file, config_file);
			break;
		case HC:
			std::cout<<"Testing HC"<<std::endl;
			algo = new hc(aapot_file, config_file);
			break;
		case ES:
			std::cout<<"Testing ES"<<std::endl;
			algo = new es(aapot_file, config_file);
			break;
		default:
			std::cout<<"Not a valid algorithm"<<std::endl;	
		}

		/* load algorithm run parameters */
		algo->setup_algo_params();

		/* load all possible antenna placements */
		algo->setup_ant_placements();

		/* load all antenna free space patterns */
		algo->read_free_space_patterns();

		algo->run();
	}
	catch (const exception &e)
	{
		std::cerr<<e.what() << "\n";
        return 1;
	}

	return 0;
}



