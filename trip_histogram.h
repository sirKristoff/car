/*
 * trip_histogram.h
 *
 *  Created on: 8 mar 2016
 *  @author Krzysztof Lasota
 */

#ifndef TRIP_HISTOGRAM_H_
#define TRIP_HISTOGRAM_H_

#include <map>

#include "route_type.h"
#include "trip.h"


#define INTERFACE_HANDLER(HandlerClass, HandlerName, HandlerMethods)  \
	public:  \
		class HandlerClass  \
		{  \
			friend TripHistogram;  \
			const TripHistogram* parent_;  \
			HandlerClass(const TripHistogram* parent) : parent_((parent))  {}  \
		public:  \
			HandlerMethods  \
		}; \
		const HandlerClass& HandlerName() const { return HandlerName##_; }  \
	protected:  \
		const HandlerClass HandlerName##_;  \
	public:


/**
 *
 */
class TripHistogram
{
public:
	typedef  RouteType       bin_type;     ///< bucket category
	typedef  Trip::Distance  value_type;   ///< categorized value (distance on trip)
	typedef  double          percent_type; ///< bucket's percentage

protected:
	typedef  std::map<bin_type, value_type, lessRtDs>    ValueHistogram;    ///< distance distribution
	typedef  std::map<bin_type, percent_type, lessRtDs>  PercentHistogram;  ///< percentage of distance distribution

public:
	TripHistogram();

	friend const Trip& operator>>(const Trip& trip, TripHistogram& hist);

	/** @fn Value value() const
	 * Interface handler for value_type data in TripHistogram::TripHistogram
	 */
	INTERFACE_HANDLER( Value, value,
		typedef  typename ValueHistogram::value_type      value_type;
		typedef  typename ValueHistogram::const_iterator  iterator;
		typedef  typename ValueHistogram::const_iterator  const_iterator;

		TripHistogram::value_type operator[]( const TripHistogram::bin_type& rt ) const;
		ValueHistogram::const_iterator begin() const;
		ValueHistogram::const_iterator end() const;
		TripHistogram::value_type overall() const;
		TripHistogram::value_type ac() const;
	)

	/** @fn Percent percent() const
	 * Interface handler for TripHistogram::percent_type data
	 */
	INTERFACE_HANDLER( Percent, percent,
		typedef  typename PercentHistogram::value_type      value_type;
		typedef  typename PercentHistogram::const_iterator  iterator;
		typedef  typename PercentHistogram::const_iterator  const_iterator;

		PercentHistogram::const_iterator begin() const;
		PercentHistogram::const_iterator end() const;
		TripHistogram::percent_type ac() const;
	)


protected:
	void clear();

	ValueHistogram valHist_;
	PercentHistogram perHist_;
	value_type acValue_;
};

#endif /* TRIP_HISTOGRAM_H_ */
