#include <gtest/gtest.h>
#include "trip.h"

#include <sstream>
#include <string>


TEST(TripTest, Tc01_Empty)
{
{
	Trip trip;
	std::istringstream istr("");

	istr >> trip;

	EXPECT_EQ( 0, (trip.end() - trip.begin()) );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}

{
	Trip trip;
	std::istringstream istr("123");

	istr >> trip;

	EXPECT_EQ( 0, (trip.end() - trip.begin()) );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}
}


TEST(TripTest, Tc02_Fist_faulty_segment)
{
	Trip trip;
	std::istringstream istr("NotARouteType");

	istr >> trip;

	EXPECT_EQ( 0, (trip.end() - trip.begin()) );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}


TEST(TripTest, Tc03_Fist_segment_wrong_type)
{
	Trip trip;
	std::istringstream istr("M.  123  A.");

	istr >> trip;

	EXPECT_EQ( 0, (trip.end() - trip.begin()) );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}


TEST(TripTest, Tc04_Bad_mileage_between_spots)
{
	Trip trip;
	std::istringstream istr("123  A.  10");

	istr >> trip;

	EXPECT_EQ( 0, (trip.end() - trip.begin()) );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}


TEST(TripTest, Tc05_Simple_one_route)
{
	Trip trip;
	std::istringstream istr("111  A.  123");

	istr >> trip;

	EXPECT_EQ( 1, (trip.end() - trip.begin()) );
	EXPECT_TRUE( istr.eof() );
	EXPECT_FALSE( istr.fail() );
}


TEST(TripTest, Tc06_Count_n_routes)
{
	Trip trip;
	std::istringstream istr(
			"011 \n"
			"A.  \n"
			"101 \n"
			"M.  \n"
			"201 \n"
			"D.  \n"
			"301");

	istr >> trip;

	EXPECT_EQ( 3, (trip.end() - trip.begin()) );
	EXPECT_TRUE( istr.eof() );
	EXPECT_FALSE( istr.fail() );
}


TEST(TripTest, Tc07_Empty_lines)
{
	Trip trip;
	std::istringstream istr(
			"111\n"
			"\n"
			"A. \t\n"
			"123 \n"
			" ");

	istr >> trip;

	EXPECT_EQ( 1, (trip.end() - trip.begin()) );
	EXPECT_TRUE( istr.eof() );
	EXPECT_FALSE( istr.fail() );
}


TEST(TripTest, Tc08_Faulty_route_type_after_good_routes)
{
	Trip trip;
	std::istringstream istr(
			"001 \n"
			"A.  \n"
			"101 \n"
			"FaultyRouteType \n"
			"201");

	istr >> trip;

	EXPECT_EQ( 0, (trip.end() - trip.begin()) );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}


TEST(TripTest, Tc09_Faulty_spot_after_good_routes)
{
	Trip trip;
	std::istringstream istr(
			"001 \n"
			"A.  \n"
			"101 \n"
			"M.  \n"
			"D.  \n"
			"301");

	istr >> trip;

	EXPECT_EQ( 0, (trip.end() - trip.begin()) );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}
