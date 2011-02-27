// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2007-2009, Geodan, Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_MAKE_HPP
#define BOOST_GEOMETRY_ALGORITHMS_MAKE_HPP

#include <boost/geometry/algorithms/assign.hpp>

#include <boost/geometry/geometries/concepts/check.hpp>

namespace boost { namespace geometry
{

/*!
\brief Construct a geometry
\ingroup make
\details
\note It does not work with array-point types, like int[2]
\tparam Geometry \tparam_geometry
\tparam Type \tparam_numeric to specify the coordinates
\param c1 \param_x
\param c2 \param_y
\return The constructed geometry, here: a 2D point

\qbk{distinguish, 2 coordinate values}
\qbk{
[heading Example]
[make_2d_point] [make_2d_point_output]

[heading See also]
\* [link geometry.reference.algorithms.assign.assign_3_2_coordinate_values assign]
}
*/
template <typename Geometry, typename Type>
inline Geometry make(Type const& c1, Type const& c2)
{
    concept::check<Geometry>();

    Geometry geometry;
    dispatch::assign
        <
            typename tag<Geometry>::type,
            Geometry,
            geometry::dimension<Geometry>::type::value
        >::apply(geometry, c1, c2);
    return geometry;
}

/*!
\brief Construct a geometry
\ingroup make
\tparam Geometry \tparam_geometry
\tparam Type \tparam_numeric to specify the coordinates
\param c1 \param_x
\param c2 \param_y
\param c3 \param_z
\return The constructed geometry, here: a 3D point

\qbk{distinguish, 3 coordinate values}
\qbk{
[heading Example]
[make_3d_point] [make_3d_point_output]

[heading See also]
\* [link geometry.reference.algorithms.assign.assign_4_3_coordinate_values assign]
}
 */
template <typename Geometry, typename Type>
inline Geometry make(Type const& c1, Type const& c2, Type const& c3)
{
    concept::check<Geometry>();

    Geometry geometry;
    dispatch::assign
        <
            typename tag<Geometry>::type,
            Geometry,
            geometry::dimension<Geometry>::type::value
        >::apply(geometry, c1, c2, c3);
    return geometry;
}

template <typename Geometry, typename Type>
inline Geometry make(Type const& c1, Type const& c2, Type const& c3, Type const& c4)
{
    concept::check<Geometry>();

    Geometry geometry;
    dispatch::assign
        <
            typename tag<Geometry>::type,
            Geometry,
            geometry::dimension<Geometry>::type::value
        >::apply(geometry, c1, c2, c3, c4);
    return geometry;
}



/*!
\brief Construct a geometry
\ingroup make
\tparam Geometry \tparam_geometry
\tparam Range \tparam_range_point
\param range \param_range_point
\return The constructed geometry, here: a linestring or a linear ring

\qbk{distinguish, with a range}
\qbk{
[heading Example]
[make_with_range] [make_with_range_output]

[heading See also]
\* [link geometry.reference.algorithms.assign.assign_2_with_a_range assign]
}
 */
template <typename Geometry, typename Range>
inline Geometry make(Range const& range)
{
    concept::check<Geometry>();

    Geometry geometry;
    append(geometry, range);
    return geometry;
}


/*!
\brief Construct a box with inverse infinite coordinates
\ingroup make
\details The make_inverse function initializes a 2D or 3D box with large coordinates, the
    min corner is very large, the max corner is very small. This is useful e.g. in combination
    with the combine function, to determine the bounding box of a series of geometries.
\tparam Geometry \tparam_geometry
\return The constructed geometry, here: a box

\qbk{
[heading Example]
[make_inverse] [make_inverse_output]

[heading See also]
\* [link geometry.reference.algorithms.assign.assign_inverse assign_inverse]
}
 */
template <typename Geometry>
inline Geometry make_inverse()
{
    concept::check<Geometry>();

    Geometry geometry;
    dispatch::assign_inverse
        <
            typename tag<Geometry>::type,
            Geometry
        >::apply(geometry);
    return geometry;
}

/*!
\brief Construct a geometry with its coordinates initialized to zero
\ingroup make
\details The make_zero function initializes a 2D or 3D point or box with coordinates of zero
\tparam Geometry \tparam_geometry
\return The constructed and zero-initialized geometry
 */
template <typename Geometry>
inline Geometry make_zero()
{
    concept::check<Geometry>();

    Geometry geometry;
    dispatch::assign_zero
        <
            typename tag<Geometry>::type,
            Geometry
        >::apply(geometry);
    return geometry;
}

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_MAKE_HPP
