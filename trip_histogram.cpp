#include "trip_histogram.h"

#include <cassert>

#include "route_type.h"


void TripHistogram::clear()
{
	acValue_ = value_type(0);

	for( bin_type routeType : allRouteType )
	{
		valHist_[routeType] = value_type(0);
		perHist_[routeType] = percent_type(0);
	}
}

TripHistogram::TripHistogram()
 : value_(this), percent_(this)
{
	clear();
}


const Trip& operator>>(const Trip& trip, TripHistogram& hist)
{
	// clear data before computations
	// if overall distance not zero
	if( hist.valHist_[RouteType::U] != TripHistogram::value_type(0) )
		hist.clear();

	/// Value (Distance) histogram
	for( const Trip::value_type& route : trip )
	{
		hist.valHist_[route.routeType] += route.distance;
		hist.valHist_[RouteType::U] += route.distance;  // sum overall distance
//		INF("hist.valHist_[" << route.distance << "] += " << route.routeType << " := " << hist.valHist_[route.routeType]);
	}


	const TripHistogram::value_type overallDistance= hist.valHist_.at(RouteType::U);

	if( 0 == overallDistance ) {
		hist.clear();
		return trip;
	}


	/// Percent Histogram
	for( const RouteType& routeType : allRouteType )
		if( routeType != RouteType::U )
			hist.perHist_[routeType] =
					TripHistogram::percent_type(100) *
					hist.valHist_.at(routeType) / overallDistance;

	/// AC
	// TODO: implement

	return trip;
}



TripHistogram::value_type
TripHistogram::Value::operator[](
		const TripHistogram::bin_type& rt ) const
{
	return parent_->valHist_.at(rt);
}


TripHistogram::ValueHistogram::const_iterator
TripHistogram::Value::begin() const
{
	// Hopefully std::map::begin() point to element with the least key
	assert(RouteType::U == parent_->valHist_.begin()->first);
	return ++(parent_->valHist_.begin());
}

TripHistogram::ValueHistogram::const_iterator
TripHistogram::Value::end() const
{
	return parent_->valHist_.end();
}

TripHistogram::value_type
TripHistogram::Value::overall() const
{
	return  parent_->valHist_.at(RouteType::U);
}

TripHistogram::value_type
TripHistogram::Value::ac() const
{
	return parent_->acValue_;
}

TripHistogram::PercentHistogram::const_iterator
TripHistogram::Percent::begin() const
{
	// Hopefully std::map::begin() point to element with the least key
	assert(RouteType::U == parent_->perHist_.begin()->first);
	return ++(parent_->perHist_.begin());
}

TripHistogram::PercentHistogram::const_iterator
TripHistogram::Percent::end() const
{
	return parent_->perHist_.end();
}

TripHistogram::percent_type
TripHistogram::Percent::ac() const
{
	if( 0 != parent_->value().overall() )
		return
			percent_type(100) *
			parent_->acValue_ /  // TODO: consider percent_type is integer
			parent_->value().overall();
	return
			percent_type(0);
}

