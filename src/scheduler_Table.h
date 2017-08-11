/*
 * scheduler_Table.h
 *
 *  Created on: 10.07.2013
 *      Author: gruenewa
 */

#ifndef SCHEDULER_TABLE_H_
#define SCHEDULER_TABLE_H_

#include <string>
#include <vector>

namespace scheduler {
namespace table {

std::string
generate_header();

std::string
generate_line();

std::string
generate_column( std::string descr
		        , double acc_time
		        , double avg_time
		        , double avg_perc
		        , double min_time
		        , double max_time);

std::string
generate_column( std::string descr
		       , std::vector<double> timings
		       , double tot_avg
		       );

std::string
generate_footer();

}	// namespace table
}	// namespace scheduler

#endif /* SCHEDULER_TABLE_H_ */
