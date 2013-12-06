#ifndef INDIVIDUAL_HPP_INCLUDED
#define INDIVIDUAL_HPP_INCLUDED
#include<ant_config.hpp>
#include<evaluation.hpp>
#include<string>
#include<vector>
#include<boost/shared_ptr.hpp>
class individual {
public:
	float fitness; /*average over all runs*/
    std::string nec_file;
	std::vector<ant_config_ptr> ant_configs;
    evaluation_ptr eval;

	individual(void);
    ~individual(void);
};
typedef boost::shared_ptr<individual> individual_ptr;
#endif
