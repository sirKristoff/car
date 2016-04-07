/*
 * main.cpp
 *
 *  Created on: 30 maj 2015
 *      Author: Krzysztof Lasota
 */

#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#include <utility>


#include "trace.h"
#include "route_type.h"
#include "trip.h"


using namespace std;


const char* hRule= "----------------------";


using Distance = Trip::Distance;

typedef map<RouteType, Distance, lessRtDs> DistanceHistogram;
typedef map<RouteType, double, lessRtDs> PercentHistogram;


void build_trip_histogram(const Trip& trip, DistanceHistogram& distanceHistogram);
void make_percent_histogram(const DistanceHistogram& distanceHistogram, PercentHistogram& percentHistogram);
double calc_ac_distance(const Trip& trip);
void print_route_stats(const DistanceHistogram& distanceHistogram,
		const PercentHistogram& percentHistogram,
		const char* label,
		const vector<RouteType> routeTypeList);
double print_stile_route_stats(const DistanceHistogram& distanceHistogram,
		const PercentHistogram& percentHistogram,
		const char* label,
		const vector<RouteType> routeTypeList);


int main()
{

	Trip trip;
	DistanceHistogram tripHistogram;
	PercentHistogram percentHistogram;

	if( !(cin >> trip) ){
		WRN("Fail to load trip");
	}

DBG(
	INF("Routes:");
	for( auto& route : trip )
	{
		INF(route.distance << " " << route.routeType);
	}
)


	build_trip_histogram(trip, tripHistogram);

DBG(
	INF("Histogram:");
	for ( RouteType routeType : allRouteType )
		if( routeType != RouteType::U )
			INF(routeType << " " << tripHistogram[routeType] << " km");

	INF("Common distance: " << tripHistogram[RouteType::U] << " km");
)


	make_percent_histogram(tripHistogram, percentHistogram);


	cout << "Statystyki: " << endl;
	print_route_stats( tripHistogram, percentHistogram, "M",
			{ RouteType( RoadType::M, DrivingStyle::Eco ),
			  RouteType( RoadType::M, DrivingStyle::Norm ),
			  RouteType( RoadType::M, DrivingStyle::Dynamic ) } );
	print_route_stats(tripHistogram, percentHistogram, "D",
			{ RouteType(RoadType::D, DrivingStyle::Eco),
			  RouteType(RoadType::D, DrivingStyle::Norm),
			  RouteType(RoadType::D, DrivingStyle::Dynamic)});
	print_route_stats(tripHistogram, percentHistogram, "A",
			{ RouteType(RoadType::A, DrivingStyle::Eco),
			  RouteType(RoadType::A, DrivingStyle::Norm),
			  RouteType(RoadType::A, DrivingStyle::Dynamic)});

	cout << hRule << std::endl
		 << tripHistogram[RouteType::U] << " km" << std::endl << std::endl;

	double stylePercentageSum = 0.0;
	stylePercentageSum += 1.0*
			print_stile_route_stats(tripHistogram, percentHistogram, "1.0: `_'",
					{ RouteType( RoadType::M, DrivingStyle::Eco ),
					  RouteType( RoadType::D, DrivingStyle::Eco ),
					  RouteType( RoadType::A, DrivingStyle::Eco ) });
	stylePercentageSum += 2.0*
				print_stile_route_stats(tripHistogram, percentHistogram, "2.0: `.'",
					{ RouteType( RoadType::M, DrivingStyle::Norm ),
					  RouteType( RoadType::D, DrivingStyle::Norm ),
					  RouteType( RoadType::A, DrivingStyle::Norm )});
	stylePercentageSum += 3.0*
				print_stile_route_stats(tripHistogram, percentHistogram, "3.0: `:'",
					{ RouteType( RoadType::M, DrivingStyle::Dynamic ),
					  RouteType( RoadType::D, DrivingStyle::Dynamic ),
					  RouteType( RoadType::A, DrivingStyle::Dynamic )});

	cout << "avg: " << stylePercentageSum/100 << " style: `"
		 << ((stylePercentageSum<500.0/3.0) ? "_" : ((stylePercentageSum<700.0/3.0)? ".":":")) << "'" << std::endl << std::endl;

	cout << "AC: " <<  setprecision(3) << 100.0 * calc_ac_distance(trip) / tripHistogram[RouteType::U] << "%" << std::endl;
	return 0;
}



/// DONE: operator>>(const Trip& trip, TripHistogram& hist)
void build_trip_histogram(const Trip& trip, DistanceHistogram& distanceHistogram)
{
	for( RouteType routeType : allRouteType ){
		distanceHistogram[routeType] = Distance();
		INF("Init histogram : routeType : " << routeType << " := " << distanceHistogram[routeType] );
	}

	for( const auto& route : trip ){
		distanceHistogram[route.routeType] += route.distance;
		distanceHistogram[RouteType::U] += route.distance;
		INF("distanceHistogram[" << route.routeType << "] += " << route.distance << " := " << distanceHistogram[route.routeType]);
	}
}

/// DONE: operator>>(const Trip& trip, TripHistogram& hist)
void make_percent_histogram(const DistanceHistogram& distanceHistogram, PercentHistogram& percentHistogram)
{
	const Distance commonDistance= distanceHistogram.at(RouteType::U);

	for( auto routeType : allRouteType )
		if( routeType != RouteType::U )
			percentHistogram[routeType] = 100.0 * distanceHistogram.at(routeType) / commonDistance;
}

#include <algorithm>

double calc_ac_distance(const Trip& trip)
{
	double acDistance= 0.0;
	std::for_each(
			trip.begin(), trip.end(),
			[&](const Trip::Route& route){ if( route.routeType.ac ) acDistance += route.distance; });

	return acDistance;
}

void print_route_stats(const DistanceHistogram& distanceHistogram,
		const PercentHistogram& percentHistogram,
		const char* label,
		const vector<RouteType> routeTypeList)
{
	Distance routeDistance =
			accumulate(
					routeTypeList.begin(), routeTypeList.end(), Distance(0),
					[&](Distance seed, RouteType rt){ return seed+distanceHistogram.at(rt); });

	cout << hRule << endl;
	cout.width(3); cout << left << label << " ";
	cout.width(5); cout << right << routeDistance << " km ";
	cout.width(6); cout << setprecision(4) << 100.0 * routeDistance / distanceHistogram.at(RouteType::U) << "%" << std::endl;

	cout << hRule << endl;

	for( RouteType routeType : routeTypeList ) {
		cout.width(3); cout << left << routeType << " ";
		cout.width(5); cout << right << distanceHistogram.at(routeType) << " km ";
		cout.width(6); cout << setprecision(4) << percentHistogram.at(routeType) << "%" << std::endl;
	}
}

double print_stile_route_stats(const DistanceHistogram& distanceHistogram,
		const PercentHistogram& percentHistogram,
		const char* label,
		const vector<RouteType> routeTypeList)
{
	double stileRoutePercent =
			accumulate(
					routeTypeList.begin(), routeTypeList.end(), double(0),
					[&](double seed, RouteType rt){ return seed+percentHistogram.at(rt); });
	cout << label << "  " << setprecision(4)  << stileRoutePercent << "%" << endl;

	return stileRoutePercent;
}
