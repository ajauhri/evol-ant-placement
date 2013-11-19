#include "eap_resources.hpp"
#include<sstream>
#include<string>
#include<iomanip>
#include<algorithm>
#include<cctype>

namespace eap
{
	a_map algorithms_map = create_algorithms_map();

	float randf(float start, float end)
	{
		return (float) (((rand() % 10000) * (end - start))/10000.0 + start);
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
	static a_map create_algorithms_map()
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
		return algorithms_map[get_svalue("algorithm")]; 
	}
}
