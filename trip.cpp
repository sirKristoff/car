#include "trip.h"

#include "trace.h"
#include "spot.h"
#include <algorithm>
#include <sstream>
#include <string>


Trip::Trip()
{
}

namespace {
template< typename T >
std::istream& putback(std::istream& istr, const T& obj)
{
	std::ostringstream sout;
	sout << obj;

	std::string serialized = sout.str();

	std::for_each(serialized.rbegin(), serialized.rend(),
			[&istr](char c){istr.putback(c);});

	return istr;
}
} // namespace


#define ASSERT_BADBIT  \
	if( istr.bad() ){  \
		FATAL("L:"<<line<<": non-recoverable error has occurred");  \
		return istr;  }

#define FAIL_PARSE(msg)  \
	istr.clear(std::ios_base::failbit);  \
	ERR(msg);  \
	trip.trip_.clear();


std::istream& operator>>( std::istream& istr, Trip& trip )
{
	Spot previousSpot = Spot();
	Spot nextSpot = Spot();
	Trip::Route currentRoute;
	unsigned line= 0;

	if( ++line, !(istr >> previousSpot) )
	{
		ASSERT_BADBIT;

		if( istr.fail() )
		{
			istr.clear();
			std::string faultySpot;
			istr >> faultySpot;
			putback(istr, faultySpot);

			FAIL_PARSE("L:"<<line<<": Spot segment unknown format:"
				" `" << faultySpot <<"'");
			return istr;
		}
	}

	while( ! istr.eof() )
	{
		/// RouteType segment
		if( ++line,  !(istr >> currentRoute.routeType).good() )
		{/// begin Error cases
			ASSERT_BADBIT;

			if( istr.eof() ){
				if( istr.fail() ){
					istr.clear(std::ios_base::eofbit);  // set only eof bit
					INF("L:"<<line<<": Empty line on the end of file");
					continue;
				}
				else{
					// RouteType segment closed file
					putback(istr, currentRoute.routeType); // push it back to stream

					FAIL_PARSE("L:"<<line<<": Spot segment is missing following"
						" valid RouteType: `" << currentRoute.routeType <<"'");
					return istr;
				}
			}
			else
			if( istr.fail() )
			{ // Faulty RouteType format
				istr.clear();
				std::string faultyRouteType;
				istr >> faultyRouteType;
				putback(istr, faultyRouteType);

				FAIL_PARSE("L:"<<line<<": RouteType segment unknown format:"
					" `" << faultyRouteType <<"'");
				return istr;
			}
		}/// end Error cases


		/// Spot segment
		if( ++line, !(istr >> nextSpot) )
		{/// begin Error cases
			ASSERT_BADBIT;

			if( istr.eof() )
			{ // nothing to read
				putback(istr, currentRoute.routeType); // push it back to stream

				FAIL_PARSE("L:"<<line<<": Spot segment is missing");
				return istr;
			}

			if( istr.fail() )
			{ // Faulty RouteType format
				istr.clear();
				std::string faultySpot;
				istr >> faultySpot;

				putback(istr, faultySpot);
				putback(istr, "\n");
				putback(istr, currentRoute.routeType);

				FAIL_PARSE("L:"<<line<<": Spot segment unknown format:"
					" `" << faultySpot <<"'");
				return istr;
			}
		}

		if( nextSpot <= previousSpot )
		{
			putback(istr, nextSpot);
			putback(istr, "\n");
			putback(istr, currentRoute.routeType);

			FAIL_PARSE("L:"<<line<<": Bad mileage of current Spot: "
				"(previousSpot, currentSpot)= "
				"("<<previousSpot<<", "<<nextSpot<<")");
			return istr;
		}/// end Error cases


		currentRoute.distance = nextSpot - previousSpot;
		previousSpot = nextSpot;
		nextSpot = Spot();

		trip.trip_.push_back(currentRoute);
	}


	// no Route was extracted -> set fail bit
	if( trip.trip_.empty() )
	{ // TODO: support non empty container on procedure's entry
		putback(istr, previousSpot);

		FAIL_PARSE("No Route was extracted");
	}

	return istr;
}

#undef ASSERT_BADBIT
#undef FAIL_PARSE
