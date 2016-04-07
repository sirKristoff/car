/*
 * route_type.h
 *
 *  Created on: 22 lut 2016
 *      Author: Krzysztof Lasota
 */

#ifndef ROUTE_TYPE_H_
#define ROUTE_TYPE_H_

#include <iostream>
#include <string>
#include <vector>



/** @brief Type of surroundings (road setup).
 */
enum class RoadType{
	U, ///< Undetermined
	M, ///< City
	D, ///< Local road
	A  ///< Highway
};

/** @brief Driving Style.
 *
 * Acceleration, average speed, frequency of stops.
 */
enum class DrivingStyle{
	Eco,
	Norm,
	Dynamic
};


/** @brief Combined info about route conditions
 *
 * @note Undetermined rt field make irrelevant rest of fields.
 */
struct RouteType
{
	typedef  unsigned  hash_type;

	RoadType rt;      ///< Type of surroundings (road setup)
	DrivingStyle ds;  ///< Driving Style
	bool ac;          ///<

	RouteType(); /**< Create Undetermined RouteType */
	RouteType(RoadType rt_, DrivingStyle ds_);
	RouteType(RoadType rt_, DrivingStyle ds_, bool ac_);


	static const hash_type rt_mask;
	static const hash_type ds_mask;
	static const hash_type ac_mask;

	/**
	 * @brief Integer identifier of all route conditions.
	 */
	hash_type hash() const;

	/**
	 * @brief Unique identifier for all masked route conditions.
	 */
	hash_type hash(const hash_type mask) const;
	/**
	 * @brief Unique identifier for all EXCEPT masked route conditions.
	 */
	hash_type hash_mask(const hash_type mask) const;


	/** @brief Check if RouteType is not Undetermined
	 */
	operator bool() const;

	/**
	 * @returns true  All route conditions are equal
	 *                or two RouteTypes are Undetermined.
	 */
	friend bool operator==(const RouteType& lhs, const RouteType& rhs);
	friend bool operator!=(const RouteType& lhs, const RouteType& rhs);


	friend std::ostream& operator<<(std::ostream& ostr, const RouteType& rt);
	friend std::istream& operator>>(std::istream& istr, RouteType& rt);

	static RouteType U; ///< Undetermined RouteType

protected:
	std::string serialize() const;
};


extern const std::vector<RouteType> allRouteType;


struct lessRtDs
{
	bool operator()(const RouteType& lhs, const RouteType& rhs) const
	{
		if( ! lhs || ! rhs )
			return lhs.rt < rhs.rt;

		return    lhs.hash(RouteType::rt_mask|RouteType::ds_mask)
				< rhs.hash(RouteType::rt_mask|RouteType::ds_mask);
	}
};


#endif /* ROUTE_TYPE_H_ */
