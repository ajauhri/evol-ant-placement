#include<eap_resources.hpp>
#include<lua_cmds.hpp>
#include<sstream>
#include<string>
#include<iomanip>
#include<algorithm>
#include<cctype>
#include<random>

namespace eap
{
	static a_map algorithms_map = create_algorithms_map();

	float randf(float start, float end)
	{
		return (float) (((rand() % 10000) * (end - start))/10000.0 + start);
	}


    unsigned int rand(unsigned int a, unsigned int b)
    {
        std::uniform_int_distribution<> dis(a,b);
        return dis(eap::gen);
    }

    unsigned int rand()
    {
        std::uniform_int_distribution<> dis;
        return dis(eap::gen);
    }

	bool to_bool(std::string str) 
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		std::istringstream is(str);
		bool b;
		is >> std::boolalpha >> b;
		return b;
	}

	/*
	 * @desc Creates a static map of available algorithms
	 * @return a map with a mapping of algorithm names to its ALGO_ID
	 */
	a_map create_algorithms_map()
	{
		a_map map;
		map["GA"] = GA;
		map["SA"] = SA;
		map["HC"] = HC;
		map["ES"] = ES;
		return map;
	}

	int get_algorithm()
	{
		std::map<std::string, algorithms>::iterator it = algorithms_map.find(get_svalue("algorithm"));
		if(it != algorithms_map.end())
			return algorithms_map[get_svalue("algorithm")];
		else 
			return -1;
	}

    int fitness_sort(const individual_ptr &ind1, const individual_ptr &ind2)
    {
        return ind1->fitness < ind2->fitness;
    }

}
