#include "route_type.h"

#include "trace.h"
#include <algorithm>


RouteType RouteType::U = RouteType();


const std::vector<RouteType> allRouteType =
	{RouteType::U,
	 RouteType(RoadType::M, DrivingStyle::Eco),
	 RouteType(RoadType::M, DrivingStyle::Norm),
	 RouteType(RoadType::M, DrivingStyle::Dynamic),
	 RouteType(RoadType::D, DrivingStyle::Eco),
	 RouteType(RoadType::D, DrivingStyle::Norm),
	 RouteType(RoadType::D, DrivingStyle::Dynamic),
	 RouteType(RoadType::A, DrivingStyle::Eco),
	 RouteType(RoadType::A, DrivingStyle::Norm),
	 RouteType(RoadType::A, DrivingStyle::Dynamic)};


RouteType::RouteType()
 : rt(RoadType::U), ds(DrivingStyle::Eco), ac(false)
{}

RouteType::RouteType(
		RoadType rt_,
		DrivingStyle ds_)
 : rt(rt_), ds(ds_), ac(false)
{}

RouteType::RouteType(
		RoadType rt_,
		DrivingStyle ds_,
		bool ac_)
 : rt(rt_), ds(ds_), ac(ac_)
{}


const RouteType::hash_type RouteType::rt_mask = RouteType::hash_type(0x00F);
const RouteType::hash_type RouteType::ds_mask = RouteType::hash_type(0x0F0);
const RouteType::hash_type RouteType::ac_mask = RouteType::hash_type(0x100);

namespace {

template <RouteType::hash_type I>
struct hash_offset    { enum { value = 1 + hash_offset< I/2 >::value }; };
template <>
struct hash_offset<0> { enum { value = 0 }; };

}

//#include <bitset>

RouteType::hash_type RouteType::hash() const
{
//	INF( "RouteType::hash(): " << *this << " "
//		<< std::bitset<hash_offset<ac_mask>::value>(
//				(hash_type(ac?1:0) << hash_offset<ds_mask|rt_mask>::value) +
//				(hash_type(ds)     << hash_offset<rt_mask>::value) +
//				 hash_type(rt)
//			) );

	if( rt == RoadType::U )
		return 0;

	return (hash_type(ac?1:0) << hash_offset<ds_mask|rt_mask>::value) +
		   (hash_type(ds)     << hash_offset<rt_mask>::value) +
			hash_type(rt);
}

RouteType::hash_type RouteType::hash(const hash_type mask) const
{
	return (hash() & mask);
}

RouteType::hash_type RouteType::hash_mask(const hash_type mask) const
{
	return (hash() & ~mask);
}


RouteType::operator bool() const
{
	return  (this->rt != RoadType::U);
}


bool operator==(const RouteType& lhs, const RouteType& rhs)
{
	if( lhs.rt == RoadType::U && rhs.rt == RoadType::U)
		return true;

	if( lhs.rt == rhs.rt &&
		lhs.ds == rhs.ds &&
		lhs.ac == rhs.ac )
		return true;

	return false;
}

bool operator!=(const RouteType& lhs, const RouteType& rhs)
{
	return ! (lhs == rhs);
}


std::ostream& operator<<( std::ostream& ostr, const RouteType& rt )
{
	ostr << rt.serialize();
	return ostr;
}


std::istream& operator>>( std::istream& istr, RouteType& rt )
{
	std::string strRouteType;
	istr >> strRouteType;

	if( istr.bad() ){
		FATAL("non-recoverable error has occurred");
		return istr;
	}

	// strRouteType empty, only non-printable chars on the end of file
	if( istr.eof() && istr.fail() )
		return istr;

	if( istr.fail() ){
		istr.clear(std::ios_base::badbit);  // set only bad bit
		FATAL("Failed to read RouteType : `" << strRouteType << "'");
		return istr;
	}

	else{
		auto routeTypeCharIt = strRouteType.begin();

		// nothing to parse
		if( routeTypeCharIt == strRouteType.end() )
		{
//			rt.rt = RoadType::U;
			istr.setstate( std::ios_base::failbit );
			FATAL( "No RoadType to read" );
			return istr;
		}

		// --------------------------------------------------------------------
		// RoadType section
		// --------------------------------------------------------------------
		switch( *routeTypeCharIt )
		{
		case 'M' :
		case 'm' :
			rt.rt = RoadType::M; break;

		case 'D' :
		case 'd' :
			rt.rt = RoadType::D; break;

		case 'A' :
		case 'a' :
			rt.rt = RoadType::A; break;

		default :
			rt.rt = RoadType::U;
			std::for_each(strRouteType.rbegin(), strRouteType.rend(),
						  [&istr](char c){  istr.putback(c);  });
//			TODO: remove
//			auto it = strRouteType.rbegin();
//			while( it != strRouteType.rend() ){
//				std::cout<<"|"<<*it<<"|";
//				std::cout<<(bool)istr.putback(*(it++));
//				std::cout <<(bool)istr.eof()<<(bool)istr.fail()<<(bool)istr.bad();
//			}
//
			istr.clear( std::ios_base::failbit );
//			TODO: remove
//			{
//				istr.clear();
//				std::string str;
//				istr >> str;
//				std::cout << "unget: `"<<str<<"'\n";
//				istr.clear( std::ios_base::failbit );
//			}
			WRN( "Undefined RoadType : `" << *routeTypeCharIt << "'" );
			return istr;
		}
		++routeTypeCharIt;

		// --------------------------------------------------------------------
		// DrivingStyle section
		// --------------------------------------------------------------------
		if( routeTypeCharIt == strRouteType.end() )
		{
			rt.ds = DrivingStyle::Norm;
			rt.ac = false;
			return istr;
		}
		switch( *routeTypeCharIt )
		{
		case '*' :
			rt.ds = DrivingStyle::Norm;
			rt.ac = true;
			return istr;

		case '_' :
		case '-' :
			rt.ds = DrivingStyle::Eco; break;

		case '.' :
			rt.ds = DrivingStyle::Norm; break;

		case ':' :
			rt.ds = DrivingStyle::Dynamic; break;


		default :
			rt.rt = RoadType::U;
			std::for_each(strRouteType.rbegin(), strRouteType.rend(),
						  [&istr](char c){istr.putback(c);});
			istr.clear( std::ios_base::failbit );
			WRN( "Undefined DrivingStyle : `" << *routeTypeCharIt << "'" );
			return istr;
		}
		++routeTypeCharIt;

		// --------------------------------------------------------------------
		// AC section
		// --------------------------------------------------------------------
		if( routeTypeCharIt == strRouteType.end() )
			rt.ac = false;
		else if( *routeTypeCharIt == '*' )
			rt.ac = true;
		else
		{
			rt.rt = RoadType::U;
			std::for_each(strRouteType.rbegin(), strRouteType.rend(),
						  [&istr](char c){istr.putback(c);});
			istr.clear( std::ios_base::failbit );
			WRN( "Undefined AC : `" << *routeTypeCharIt << "'" );
			return istr;
		}
	}

	return istr;
}



std::string RouteType::serialize() const
{
	std::string str("");

#define PR_ROADTYPE(type,stamp) case RoadType::type : str += stamp; break
	switch( this->rt )
	{
	PR_ROADTYPE( U, 'U'; return str );
	PR_ROADTYPE( M, "M" );
	PR_ROADTYPE( D, "D" );
	PR_ROADTYPE( A, "A" );
	}
#undef PR_ROADTYPE

#define PR_DRIVINGSTYLE(type,stamp) case DrivingStyle::type : str += stamp; break
	switch( this->ds )
	{
	PR_DRIVINGSTYLE( Eco, "_" );
	PR_DRIVINGSTYLE( Norm, "." );
	PR_DRIVINGSTYLE( Dynamic, ":" );
	}
#undef PR_DRIVINGSTYLE

#define PR_AC(type,stamp) case type : str += stamp; break
	switch( unsigned(this->ac) )
	{
	PR_AC( true, "*" );
	PR_AC( false, "" );
	}
#undef PR_AC

	return (str);
}
