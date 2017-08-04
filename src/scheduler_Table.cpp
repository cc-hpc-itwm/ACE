/*
 * scheduler_Table.cpp
 *
 *  Created on: 10.07.2013
 *      Author: gruenewa
 */

#include "scheduler_Table.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits>

namespace scheduler {
namespace table {

int const floatprec(3);
int const floatmagn(5);
int const floatwidth(floatprec + 1 + floatmagn);
int const descrwidth(20);
int const totalwidth( descrwidth + 3 + 6 * (floatwidth + 2 ) );

std::string
generate_header()
{
	std::stringstream sstream;

	sstream<<"==========================================="<<std::endl;
	sstream<<"Timing information:"<<std::endl<<std::endl;

	sstream<<generate_line()<<std::endl;

	sstream<<std::fixed<<std::setprecision(floatprec);

	sstream<<"\t"
			<<std::setw(1)<<"|"
			<<std::setw(descrwidth)<<"Timer type"
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<"Avg[s]"
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<"Avg[%]"
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<"Min[s]"
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<"Max[s]"
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<"Acc[s]"
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<"Eff[%]"
			<<std::setw(2)<<" |"
			<<std::endl;

	sstream<<generate_line();

	return sstream.str();

}

std::string
generate_line()
{
	std::stringstream sstream;

	sstream<<"\t";
	for(int i(0);i<totalwidth;++i)
		sstream<<"-";

	return sstream.str();

}

std::string
generate_column( std::string descr
		        , double acc_time
		        , double avg_time
		        , double avg_perc
		        , double min_time
		        , double max_time)
{
	const double eff_perc( ( avg_time > 0. ) ? (min_time / avg_time * 100.) : 100. );

	std::stringstream sstream;

	sstream<<std::fixed<<std::setprecision(3);

	sstream<<"\t"
			<<std::setw(1)<<"|"
			<<std::setw(descrwidth)<<descr
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<avg_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<avg_perc
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<min_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<max_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<acc_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<eff_perc
			<<std::setw(2)<<" |";

	return sstream.str();
}

std::string
generate_column( std::string descr
		       , std::vector<double> timings	///<
		       , double tot_avg					///< average total time
		       )
{
	double acc_time( 0. );
	double min_time( +std::numeric_limits<double>::max() );
    double max_time( -std::numeric_limits<double>::max() );

    for( int i(0);i<timings.size();++i)
    {
    	double const timing( timings[i] );
    	acc_time += timing;
    	if (timing < min_time)
    	{
    		min_time = timing;
    	}
    	if (timing > max_time)
    	{
    		max_time = timing;
    	}
    }

    double const avg_time( acc_time / static_cast<double>( timings.size() ) );
    double const avg_perc( avg_time / tot_avg * 100. );
	double const eff_perc( ( avg_time > 0. ) ? (min_time / avg_time * 100.) : 100. );

	std::stringstream sstream;

	sstream<<std::fixed<<std::setprecision(3);

	sstream<<"\t"
			<<std::setw(1)<<"|"
			<<std::setw(descrwidth)<<descr
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<avg_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<avg_perc
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<min_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<max_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<acc_time
			<<std::setw(2)<<" |"
			<<std::setw(floatwidth)<<eff_perc
			<<std::setw(2)<<" |";

	return sstream.str();
}

std::string
generate_footer()
{
	std::stringstream sstream;

	sstream<<std::endl;

	sstream<<"==========================================="<<std::endl;

	return sstream.str();
}

} 	// namespace table
}	// namespace scheduler
