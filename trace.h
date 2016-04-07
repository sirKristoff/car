/*
 * trace.h
 *
 *  Created on: 22 lut 2016
 *      Author: krla
 */

#ifndef TRACE_H_
#define TRACE_H_

#include <iostream>


#define FATAL(msg) \
	std::cout <<__FILE__<<":"<<__LINE__<<": error: " \
	          << msg << std::endl;

#define ERR(msg) \
	std::cout <<__FILE__<<":"<<__LINE__<<": error: " \
	          << msg << std::endl;

#ifndef NDEBUG
 #define WRN(msg) \
	std::cout <<__FILE__<<":"<<__LINE__<<": warning: " \
	          << msg << std::endl;

 #define INF(msg) \
	std::cout <<__FILE__<<":"<<__LINE__<<": info: " \
	          << msg << std::endl;

 #define DBG(cmd) \
	cmd

#else // NDEBUG
 #define WRN(msg) ;
 #define INF(msg) ;
 #define DBG(cmd) ;
#endif // NDEBUG


#endif /* TRACE_H_ */
