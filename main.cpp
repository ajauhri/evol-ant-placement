// main.cpp : Entry point for the console application.
#include<iostream>
#include<fstream>
#include<time.h>
#include<string>
#include<boost/program_options.hpp>
#include<boost/filesystem.hpp>

#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<algorithm.hpp>
#include<ga.hpp>
#include<sa.hpp>
#include<es.hpp>
#include<hc.hpp>
#include<exhaust.hpp>

namespace prg_opts = boost::program_options;

std::unique_ptr<algorithm> eap::algo = NULL;

int main(int argc, char* argv[])
{
    try 
    {
        std::string lua_file;
        unsigned int runs = 0;

        prg_opts::options_description desc("\nUsage");
        desc.add_options()
            ("help,h", "view valid options")
            ("input,i", prg_opts::value<std::string>(&lua_file), "Input file")
            ("runs,r", prg_opts::value<unsigned int>(&runs), "#of runs");


        prg_opts::variables_map vm;
        prg_opts::store(prg_opts::parse_command_line(argc, argv, desc), vm);
        prg_opts::notify(vm);

        if (vm.count("help") || !vm.count("input") || !vm.count("runs"))
        {
            std::cout << desc << "\n";
            return 0;
        }

        if (!std::ifstream(lua_file.c_str()))
        {
            std::cout << "Can not open input file" << "\n";
            return 0;
        }

        eap::init_lua();
        eap::load_lua_lib(lua_file.c_str());

        switch(eap::get_algorithm())
        {
            case GA:
                std::cout<<"Running GA...\n";
                eap::algo.reset(new ga(lua_file));
                break;
            case SA:
                std::cout<<"Running SA...\n";
                eap::algo.reset(new sa(lua_file));
                break;
            case HC:
                std::cout<<"Running HC...\n";
                eap::algo.reset(new hc(lua_file));
                break;
            case ES:
                std::cout<<"Running ES...\n";
                eap::algo.reset(new es(lua_file));
                break;
            case EX:
                std::cout<<"Running EX...\n";
                eap::algo.reset(new exhaust(lua_file));
                break;
            default:
                std::cout<<"Not a valid algorithm"<<std::endl;	
        }

        /* load algorithm run parameters */
        eap::algo->setup_algo_params();

        /* load all possible antenna placements */
        eap::algo->algorithm::setup_ant_placements();

        /* load all wires from nec files */
        eap::algo->algorithm::load_nec_files();

        /*clean up previous output files */
        eap::algo->algorithm::setup_run_context();

        /* create antenna free space patterns */
        eap::algo->algorithm::write_freespace();

        /* run free space pattern files */
        eap::algo->algorithm::run_freespace();

        /* read free space pattern results */
        eap::algo->algorithm::read_freespace();

        for (unsigned int i=0; i<runs; ++i)
        {
            /* run the specific algorithm */
            eap::algo->run();
            std::string name = boost::filesystem::basename (lua_file);
            
            /* save results */
            boost::format formatter("cd " + eap::run_directory + "; tar -cjf ../../" + name + "_a" + std::to_string(eap::get_algorithm()) + "_i%02d." + "tar.bz2 *");
            std::cout<<str(formatter % i)<<"\n";
            system(str(formatter % i).c_str());
        }
    }
    catch (const std::exception &e)
    {
        std::cerr<<e.what() << "\n";
    }
    eap::close_lua();
    return 0;
}



