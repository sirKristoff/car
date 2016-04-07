#include <gtest/gtest.h>

#include "trip_histogram.h"

#include <algorithm>
#include <initializer_list>
#include <utility>
#include <vector>


class TripStub : public Trip
{
public:
	TripStub(std::initializer_list<value_type> il)
	{  trip_ = std::move(TripContainer(il));  }
};


#define RTA(road_type_, driving_style_, ac_)  \
	RouteType(RoadType::road_type_, DrivingStyle::driving_style_, ac_)
#define RT(road_type_, driving_style_)  \
	RouteType(RoadType::road_type_, DrivingStyle::driving_style_)


void checkClearHistogram(const TripHistogram& th)
{
	EXPECT_EQ(TripHistogram::value_type(0), th.value().overall());

	for( const TripHistogram::Value::value_type& rtd : th.value() )
		EXPECT_EQ(TripHistogram::value_type(0), rtd.second)
		<< "      rtd.first (RouteType) is: `" << rtd.first << "'";

	for( const TripHistogram::Percent::value_type& rtp : th.percent() )
		EXPECT_EQ(TripHistogram::percent_type(0), rtp.second)
		<< "      rtd.first (RouteType) is: `" << rtp.first << "'";


	EXPECT_EQ(TripHistogram::value_type(0), th.value().ac());
}


const std::vector<RouteType>
allRtExc(std::initializer_list<RouteType> excl)
{
	std::vector<RouteType> ret;

	for( RouteType rt : allRouteType )
		if( rt != RouteType::U
				// rt not in excl
				&& excl.end() == std::find(excl.begin(), excl.end(), rt) )

			ret.push_back(rt);

	return ret;
}


TEST(TripHistTest, Tc01_Constructor)
{
	TripHistogram th;

	checkClearHistogram(th);
}


TEST(TripHistTest, Tc02_Read_empty_trip)
{
	TripHistogram th;
	Trip emptyTrip;

	emptyTrip >> th;

	checkClearHistogram(th);
}


TEST(TripHistTest, Tc03_Read_another_trip)
{
	TripHistogram th;
	Trip emptyTrip;
	TripStub simpleTrip({  {10, {RoadType::A, DrivingStyle::Eco, true}}  });

	emptyTrip >> th;
	checkClearHistogram(th);

	simpleTrip >> th;
	EXPECT_NE(TripHistogram::value_type(0), th.value().overall());
//	EXPECT_NE(TripHistogram::value_type(0), th.value().ac());  // TODO: uncoment
//	EXPECT_NE(TripHistogram::percent_type(0), th.percent().ac());

	emptyTrip >> th;
	checkClearHistogram(th);
}


TEST(TripHistTest, Tc04a_Value_only_one_bin)
{
	TripHistogram th;
	TripStub trip({
		{10, RT(A, Eco)},
		{21, RT(A, Eco)},
		{32, RT(A, Eco)}
	});

	trip >> th;

	EXPECT_EQ(TripHistogram::value_type(63), th.value().overall());

	EXPECT_EQ(TripHistogram::value_type(63), th.value()[RT(A, Eco)]);

	for( TripHistogram::bin_type rt : allRtExc({RT(A, Eco)}) )
		EXPECT_EQ(TripHistogram::value_type(0), th.value()[rt])
		<< "      rt (RouteType) is: `" << rt << "'";
}



TEST(TripHistTest, Tc04_Value_)
{
	TripHistogram th;
	TripStub trip({
		{10, {RoadType::A, DrivingStyle::Eco}},
		{10, {RoadType::A, DrivingStyle::Eco}},
		{10, {RoadType::A, DrivingStyle::Eco}},
		{10, {RoadType::A, DrivingStyle::Eco}},
		{10, {RoadType::A, DrivingStyle::Eco}}
	});

	trip >> th;
	EXPECT_EQ(TripHistogram::value_type(50), th.value().overall());
}


TEST(TripHistTest, Tc05_Value_overall)
{
	TripHistogram th;
	TripStub trip({
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{15, {RoadType::M, DrivingStyle::Dynamic, false}}
	});

	trip >> th;
	EXPECT_EQ(TripHistogram::value_type(25), th.value().overall());
}


TEST(TripHistTest, Tc06a_Value_ac_min)
{
	TripHistogram th;
	TripStub trip({
		{10, {RoadType::A, DrivingStyle::Eco, false}},
		{11, {RoadType::A, DrivingStyle::Eco, false}}
	});

	trip >> th;
	EXPECT_EQ(TripHistogram::value_type(0), th.value().ac());
}


TEST(TripHistTest, Tc06b_Value_ac_max)
{
	TripHistogram th;
	TripStub trip({
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{15, {RoadType::A, DrivingStyle::Eco, true}}
	});

	trip >> th;
//	EXPECT_EQ(TripHistogram::value_type(25), th.value().ac()); // TODO: uncoment
}


TEST(TripHistTest, Tc06c_Value_ac_15)
{
	TripHistogram th;
	TripStub trip({
		{10, {RoadType::A, DrivingStyle::Eco, false}},
		{15, {RoadType::D, DrivingStyle::Dynamic, true}},
		{20, {RoadType::M, DrivingStyle::Dynamic, false}},
		{30, {RoadType::D, DrivingStyle::Dynamic, false}}
	});

	trip >> th;
//	EXPECT_EQ(TripHistogram::value_type(15), th.value().ac()); // TODO: uncoment
}


TEST(TripHistTest, Tc0_Constructors)
{
	TripHistogram th;
	TripStub trip({
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, false}}
	});

	trip >> th;
	EXPECT_EQ(TripHistogram::value_type(50), th.value().overall());
}


TEST(TripHistTest, Tc00_Constructors)
{
	TripHistogram th;
	TripStub trip({
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, true}},
		{10, {RoadType::A, DrivingStyle::Eco, false}}
	});

	trip >> th;
	const TripHistogram& cth = th;
//	for( auto val : cth.value() )
//		val;
//	TripHistogram::Value::iterator it = cth.value().begin();
	EXPECT_EQ(50, cth. value().overall());
}


#undef RT
#undef RTA

