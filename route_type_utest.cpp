#include <gtest/gtest.h>
#include "route_type.h"

#include <functional>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>


TEST(RouteTypeTest, Tc01_Constructors)
{
	const RoadType rt_D = RoadType::D;
	const DrivingStyle ds_N = DrivingStyle::Norm;
	const bool ac_T = true;
{
	RouteType rt;

	EXPECT_EQ(RoadType::U, rt.rt)
		<< "Default RouteType isn't Undetermined";
}
{
	RouteType rt(rt_D, ds_N);

	EXPECT_EQ(rt_D, rt.rt);
	EXPECT_EQ(ds_N, rt.ds);
	EXPECT_FALSE(rt.ac);
}
{
	RouteType rt(rt_D, ds_N, ac_T);

	EXPECT_EQ(rt_D, rt.rt);
	EXPECT_EQ(ds_N, rt.ds);
	EXPECT_EQ(ac_T, rt.ac);
}
}


TEST(RouteTypeTest, Tc02_LogicOperators)
{
{// operator bool
	RouteType rtU(RoadType::U, DrivingStyle::Norm);

	EXPECT_FALSE(bool(rtU))
		<< "Undetermined object isn't invalid";

	rtU.rt = RoadType::M;
	EXPECT_TRUE(bool(rtU))
		<< "Determined object isn't valid";
}
{// operator==
	EXPECT_TRUE( bool(RouteType(RoadType::U, DrivingStyle::Norm)
			       == RouteType(RoadType::U, DrivingStyle::Eco)) );

	EXPECT_FALSE( bool(RouteType(RoadType::U, DrivingStyle::Norm)
			        == RouteType(RoadType::A, DrivingStyle::Norm)) );

	EXPECT_TRUE( bool(RouteType(RoadType::A, DrivingStyle::Eco, true)
			       == RouteType(RoadType::A, DrivingStyle::Eco, true)) );

	EXPECT_FALSE( bool(RouteType(RoadType::A, DrivingStyle::Eco, true)
			        == RouteType(RoadType::A, DrivingStyle::Eco, false)) );

	EXPECT_FALSE( bool(RouteType(RoadType::A, DrivingStyle::Eco, true)
			        == RouteType(RoadType::M, DrivingStyle::Eco, true)) );
}
{// operator!=
	EXPECT_FALSE( bool(RouteType(RoadType::A, DrivingStyle::Eco, true)
			        != RouteType(RoadType::A, DrivingStyle::Eco, true)) );
}
}


TEST(RouteTypeTest, Tc02a_StaticUndetermined)
{
	EXPECT_EQ(RouteType(), RouteType::U);
	EXPECT_EQ(RouteType(RoadType::U, DrivingStyle::Norm), RouteType::U);
	EXPECT_NE(RouteType(RoadType::A, DrivingStyle::Eco, true), RouteType::U);
}

TEST(RouteTypeTest, Tc03_Hash)
{
{// Undetermined
	EXPECT_EQ( RouteType(RoadType::U, DrivingStyle::Norm, false).hash(),
			   RouteType(RoadType::U, DrivingStyle::Dynamic, true).hash() )
		<< "Two different Undetermined RouteTypes haven't equal hashes";

	EXPECT_EQ( RouteType::hash_type(0),
			   RouteType(RoadType::U, DrivingStyle::Dynamic, true).hash() );
}
{// equal hashes
	EXPECT_NE( RouteType(RoadType::A, DrivingStyle::Norm, false).hash(),
			   RouteType(RoadType::A, DrivingStyle::Norm, true).hash() );

	EXPECT_EQ( RouteType(RoadType::A, DrivingStyle::Norm, true).hash(),
			   RouteType(RoadType::A, DrivingStyle::Norm, true).hash() );
}
}

TEST(RouteTypeTest, Tc04_UniqueHash)
{
	std::vector<RouteType> allRt;
	for( auto rt : {RoadType::A, RoadType::D, RoadType::M})
		for( auto ds : {DrivingStyle::Dynamic, DrivingStyle::Eco, DrivingStyle::Norm})
			for( auto ac : {true, false})
				allRt.push_back(RouteType(rt, ds, ac));

	for( auto& lhs : allRt )
		for( auto& rhs : allRt )
		{
			if( lhs == rhs ){
				EXPECT_EQ( lhs.hash(), rhs.hash() )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}else{
				EXPECT_NE( lhs.hash(), rhs.hash() )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}
		}
}


TEST(RouteTypeTest, Tc05a_HashMask)
{
	std::vector<RouteType> allRt;
	for( auto rt : {RoadType::A, RoadType::D, RoadType::M})
		for( auto ds : {DrivingStyle::Dynamic, DrivingStyle::Eco, DrivingStyle::Norm})
			for( auto ac : {true, false})
				allRt.push_back(RouteType(rt, ds, ac));

{
	typedef  std::function<bool(const RouteType&,const RouteType&)>  TCaseFun;
	std::vector< std::tuple< unsigned, TCaseFun, RouteType::hash_type > > tcs;

#define TC( _id_, _case_fun_expr_, _mask_to_test_ )  \
	std::tuple<unsigned, TCaseFun, RouteType::hash_type>(  \
		_id_,  \
		[](const RouteType& lhs,const RouteType& rhs){  \
			return (_case_fun_expr_);},  \
		_mask_to_test_ )

	for( auto tc :
		{
		TC( 1, lhs.rt == rhs.rt, RouteType::rt_mask ),
		TC( 2, lhs.ds == rhs.ds, RouteType::ds_mask ),
		TC( 3, lhs.ac == rhs.ac, RouteType::ac_mask ),
		TC( 4, lhs.rt == rhs.rt && lhs.ds == rhs.ds,
				RouteType::rt_mask | RouteType::ds_mask ),
		TC( 5, lhs.ds == rhs.ds && lhs.ac == rhs.ac,
				RouteType::ds_mask | RouteType::ac_mask ),
		TC( 6, lhs.ac == rhs.ac && lhs.rt == rhs.rt,
				RouteType::ac_mask | RouteType::rt_mask )
		}
	)
		tcs.push_back(tc);
#undef TC

	for( auto id_fun_mask :  tcs )
	for( auto& lhs : allRt )
		for( auto& rhs : allRt )
		{
			if( (std::get<1>(id_fun_mask))(lhs,rhs) )
			{
				EXPECT_EQ( lhs.hash(std::get<2>(id_fun_mask)) ,
						   rhs.hash(std::get<2>(id_fun_mask))
				)
					<< "    case no. "<< std::get<0>(id_fun_mask)
					<< "  lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}
			else
			{
				EXPECT_NE( lhs.hash(std::get<2>(id_fun_mask)),
						   rhs.hash(std::get<2>(id_fun_mask))
				)
					<< "    case no. "<< std::get<0>(id_fun_mask)
					<< "  lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}
		}
}
}


TEST(RouteTypeTest, Tc05b_HashMask_exclude)
{
	std::vector<RouteType> allRt;
	for( auto rt : {RoadType::A, RoadType::D, RoadType::M})
		for( auto ds : {DrivingStyle::Dynamic, DrivingStyle::Eco, DrivingStyle::Norm})
			for( auto ac : {true, false})
				allRt.push_back(RouteType(rt, ds, ac));

{// rt_mask => rt member is pointless in hash
	for( auto& lhs : allRt )
		for( auto& rhs : allRt )
		{
			if( lhs.ds == rhs.ds && lhs.ac == rhs.ac ){
				EXPECT_EQ( lhs.hash_mask(RouteType::rt_mask) ,
						   rhs.hash_mask(RouteType::rt_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}else{
				EXPECT_NE( lhs.hash_mask(RouteType::rt_mask),
						   rhs.hash_mask(RouteType::rt_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}
		}
}
{// ds_mask => ds member is pointless in hash
	for( auto& lhs : allRt )
		for( auto& rhs : allRt )
		{
			if( lhs.rt == rhs.rt && lhs.ac == rhs.ac ){
				EXPECT_EQ( lhs.hash_mask(RouteType::ds_mask) ,
						   rhs.hash_mask(RouteType::ds_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}else{
				EXPECT_NE( lhs.hash_mask(RouteType::ds_mask),
						   rhs.hash_mask(RouteType::ds_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}
		}
}
{// ac_mask => ac member is pointless in hash
	for( auto& lhs : allRt )
		for( auto& rhs : allRt )
		{
			if( lhs.rt == rhs.rt && lhs.ds == rhs.ds ){
				EXPECT_EQ( lhs.hash_mask(RouteType::ac_mask) ,
						   rhs.hash_mask(RouteType::ac_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}else{
				EXPECT_NE( lhs.hash_mask(RouteType::ac_mask),
						   rhs.hash_mask(RouteType::ac_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}
		}
}
{// ds_mask | ac_mask => ds and ac member is pointless in hash
	for( auto& lhs : allRt )
		for( auto& rhs : allRt )
		{
			if( lhs.rt == rhs.rt ){
				EXPECT_EQ( lhs.hash_mask(RouteType::ds_mask|RouteType::ac_mask) ,
						   rhs.hash_mask(RouteType::ds_mask|RouteType::ac_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}else{
				EXPECT_NE( lhs.hash_mask(RouteType::ds_mask|RouteType::ac_mask),
						   rhs.hash_mask(RouteType::ds_mask|RouteType::ac_mask) )
					<< "    lsh: `"<<(lhs)<<"'  rsh: `"<<(rhs)<<"'";
			}
		}
}
}


TEST(RouteTypeTest, Tc06_IstreamOperator_positive)
{
#define RT(_rt_, _ds_, _ac_)  \
	RouteType(RoadType::_rt_, DrivingStyle::_ds_, bool(_ac_))
#define TC(input, _rt_, _ds_, _ac_)  \
	std::make_pair(std::string(input), RT(_rt_, _ds_, _ac_))

	std::vector< std::pair<std::string,RouteType> > tcs;
	for( auto& input_expected :
		{
		TC("A:",  A,Dynamic,0),
		TC("a.",  A,Norm,0),
		TC("a-",  A,Eco,0),

		TC("m:",  M,Dynamic,0),
		TC("M.",  M,Norm,0),
		TC("m-",  M,Eco,0),

		TC("d:",  D,Dynamic,0),
		TC("d.",  D,Norm,0),
		TC("D_",  D,Eco,0),

		TC("A_*", A,Eco,1),
		TC("M:*", M,Dynamic,1),
		TC("D.*", D,Norm,1),

		TC("A", A,Norm,0),
		TC("D", D,Norm,0),
		TC("M", M,Norm,0),

		TC("a*", A,Norm,1),
		TC("d*", D,Norm,1),
		TC("m*", M,Norm,1)
		}
	)
		tcs.push_back(input_expected);
#undef TC
#undef RT

	for( auto& input_expected : tcs )
	{
		RouteType rt;
		std::istringstream istr(input_expected.first);

		istr >> rt;

		EXPECT_TRUE( (bool)istr )
			<< " eof()=" << istr.eof()
			<< " fail()=" << istr.fail()
			<< " bad()=" << istr.bad();

		EXPECT_EQ( input_expected.second, rt);
	}
}


TEST(RouteTypeTest, Tc07_IstreamOperator_formating_negtive)
{
{ // No RoadType to read
	RouteType rt(RoadType::A, DrivingStyle::Dynamic);
	RouteType expectedRt(rt);
	std::istringstream istr(" ");

	istr >> rt;

	EXPECT_EQ( expectedRt, rt );
	EXPECT_TRUE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}
{ // Not a RoadType on input
	RouteType rt(RoadType::A, DrivingStyle::Dynamic);
	RouteType expectedRt = RouteType::U;
	std::istringstream istr("NotARoadType");

	istr >> rt;

	EXPECT_EQ( expectedRt, rt );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}
{ // Bad format of RoadType (DrivingStyle)
	RouteType rt(RoadType::A, DrivingStyle::Dynamic);
	RouteType expectedRt = RouteType::U;
	std::istringstream istr("D@");

	istr >> rt;

	EXPECT_EQ( expectedRt, rt );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}
{ // Bad format of RoadType (AC)
	RouteType rt(RoadType::A, DrivingStyle::Dynamic);
	RouteType expectedRt = RouteType::U;
	std::istringstream istr("M_?");

	istr >> rt;

	EXPECT_EQ( expectedRt, rt );
	EXPECT_FALSE( istr.eof() );
	EXPECT_TRUE( istr.fail() );
}
}



TEST(RouteTypeTest, Tc08_OstreamOperator_positive)
{
#define RT(_rt_, _ds_, _ac_)  \
	RouteType(RoadType::_rt_, DrivingStyle::_ds_, bool(_ac_))
#define TC(expected, _rt_, _ds_, _ac_)  \
	std::make_pair(std::string(expected), RT(_rt_, _ds_, _ac_))

	std::vector< std::pair<std::string,RouteType> > tcs;
	for( auto& expected_and_input :
		{
		TC("A:",  A,Dynamic,0),
		TC("A.",  A,Norm,0),
		TC("A_",  A,Eco,0),

		TC("M:",  M,Dynamic,0),
		TC("M.",  M,Norm,0),
		TC("M_",  M,Eco,0),

		TC("D:",  D,Dynamic,0),
		TC("D.",  D,Norm,0),
		TC("D_",  D,Eco,0),

		TC("A_*", A,Eco,1),
		TC("M:*", M,Dynamic,1),
		TC("D.*", D,Norm,1),

		TC("U", U,Norm,1)
		}
	)
		tcs.push_back(expected_and_input);
#undef TC
#undef RT

	for( auto& expected_and_input : tcs )
	{
		std::ostringstream ostr;
		ostr << expected_and_input.second;

		EXPECT_EQ( expected_and_input.first, ostr.str() );
	}
}
