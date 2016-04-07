/*
 * trip.h
 *
 *  Created on: 25 lut 2016
 *      Author: Krzysztof Lasota
 */

#ifndef TRIP_H_
#define TRIP_H_

#include <iostream>
#include <vector>
#include <utility>

#include "route_type.h"


class Trip
{
public:
	typedef  unsigned int        Distance;  ///< Driving distance on Route
	///
	typedef  struct Route {
		Distance distance;
		RouteType routeType;
	}                            value_type;
	typedef  std::vector<Route>  TripContainer;  ///<

	typedef  typename TripContainer::iterator        iterator;
	typedef  typename TripContainer::const_iterator  const_iterator;

protected:
	TripContainer trip_;

public:
	Trip();
	virtual ~Trip() {}

	iterator begin()  {  return trip_.begin();  }
	const_iterator begin() const  {  return trip_.begin();  }
	iterator end()  {  return trip_.end();  }
	const_iterator end() const  {  return trip_.end();  }

	friend std::istream& operator>>(std::istream& istr, Trip& rt);
};

#endif /* TRIP_H_ */
