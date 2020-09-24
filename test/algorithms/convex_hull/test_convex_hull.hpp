// Boost.Geometry (aka GGL, Generic Geometry Library)
// Unit Test

// Copyright (c) 2007-2015 Barend Gehrels, Amsterdam, the Netherlands.

// This file was modified by Oracle on 2014, 2015.
// Modifications copyright (c) 2014-2015 Oracle and/or its affiliates.

// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle
// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_TEST_CONVEX_HULL_HPP
#define BOOST_GEOMETRY_TEST_CONVEX_HULL_HPP

#include <boost/variant/variant.hpp>

#include <geometry_test_common.hpp>

#include <boost/geometry/algorithms/convex_hull.hpp>
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/is_empty.hpp>
#include <boost/geometry/algorithms/num_points.hpp>
#include <boost/geometry/algorithms/perimeter.hpp>

#include <boost/geometry/strategies/strategies.hpp>
#include <boost/geometry/strategies/area/services.hpp>

#include <boost/geometry/io/wkt/wkt.hpp>

#include <boost/geometry/geometries/polygon.hpp>


template <typename Geometry, typename Hull, typename AreaStrategy>
void check_convex_hull(Geometry const& geometry, Hull const& hull,
                      std::size_t /*size_original*/, std::size_t size_hull,
                      double expected_area, double expected_perimeter,
                      bool reverse, AreaStrategy area_strategy)
{
    std::size_t n = bg::num_points(hull);

    BOOST_CHECK_MESSAGE(n == size_hull,
        "convex hull: " << bg::wkt(geometry)
        << " -> " << bg::wkt(hull)
        << " type "
        << (typeid(typename bg::coordinate_type<Hull>::type).name())
        << " -> Expected: " << size_hull
        << " detected: " << n);


    // We omit this check as it is not important for the hull algorithm
    // BOOST_CHECK(bg::num_points(geometry) == size_original);

    auto ah = bg::area(hull, area_strategy);

    if (reverse)
    {
        ah = -ah;
    }

    BOOST_CHECK_CLOSE(ah, expected_area, 0.001);

    if ( expected_perimeter >= 0 )
    {
        auto ph = bg::perimeter(hull);

        BOOST_CHECK_CLOSE(ph, expected_perimeter, 0.001);
    }
}

namespace resolve_variant {

struct closure_visitor : public boost::static_visitor<bg::closure_selector>
{
    template <typename Geometry>
    bg::closure_selector operator()(Geometry const&) const
    {
        return bg::closure<Geometry>::value;
    }
};

template <typename Geometry>
inline bg::closure_selector get_closure(Geometry const&)
{
    return bg::closure<Geometry>::value;
}

template <BOOST_VARIANT_ENUM_PARAMS(typename T)>
inline bg::closure_selector get_closure(boost::variant<BOOST_VARIANT_ENUM_PARAMS(T)> const& v)
{
    return boost::apply_visitor(closure_visitor(), v);
}

} // namespace resolve_variant

template <typename Hull, typename Strategy, typename AreaStrategy, typename Geometry>
void test_convex_hull(Geometry const& geometry,
                      std::size_t size_original, std::size_t size_hull_closed,
                      double expected_area, double expected_perimeter,
                      bool reverse, bool use_only_strategy = false)
{
    bool const is_original_closed = resolve_variant::get_closure(geometry) != bg::open;
    static bool const is_hull_closed = bg::closure<Hull>::value != bg::open;
    
    // convex_hull_insert() uses the original Geometry as a source of the info
    // about the order and closure
    std::size_t const size_hull_from_orig = is_original_closed ?
        size_hull_closed : size_hull_closed - 1;
    std::size_t const size_hull = is_hull_closed ? size_hull_closed :
        size_hull_closed - 1;

    Hull hull;

    if (!use_only_strategy)
    {
        // Test version with output iterator
        bg::detail::convex_hull::convex_hull_insert(geometry,
                                                    std::back_inserter(hull.outer()));
        check_convex_hull(geometry, hull, size_original, size_hull_from_orig,
                          expected_area, expected_perimeter, reverse,
                          AreaStrategy());

        // Test version with ring as output
        bg::clear(hull);
        bg::convex_hull(geometry, hull.outer());
        check_convex_hull(geometry, hull, size_original, size_hull, expected_area,
                          expected_perimeter, false, AreaStrategy());

        // Test version with polygon as output
        bg::clear(hull);
        bg::convex_hull(geometry, hull);
        check_convex_hull(geometry, hull, size_original, size_hull, expected_area,
                          expected_perimeter, false, AreaStrategy());
    }
    // Test version with strategy
    bg::clear(hull);
    bg::convex_hull(geometry, hull.outer(), Strategy());
    check_convex_hull(geometry, hull, size_original, size_hull, expected_area,
        expected_perimeter, false, AreaStrategy());

    // Test version with output iterator and strategy
    bg::clear(hull);
    bg::detail::convex_hull::convex_hull_insert(geometry,
        std::back_inserter(hull.outer()), Strategy());
    check_convex_hull(geometry, hull, size_original, size_hull_from_orig,
        expected_area, expected_perimeter, reverse, AreaStrategy());

}


template
<
    typename Geometry,
    typename Strategy,
    typename AreaStrategy,
    bool Clockwise,
    bool Closed
>
void test_geometry_order(std::string const& wkt,
                         std::size_t size_original, std::size_t size_hull_closed,
                         double expected_area, double expected_perimeter = -1.0,
                         bool use_only_strategy = false)
{
    typedef bg::model::polygon
        <
            typename bg::point_type<Geometry>::type,
            Clockwise,
            Closed
        > hull_type;

    Geometry geometry;
    bg::read_wkt(wkt, geometry);


    test_convex_hull<hull_type, Strategy, AreaStrategy>(geometry, size_original,
        size_hull_closed, expected_area, expected_perimeter, !Clockwise,
        use_only_strategy);

    boost::variant<Geometry> v(geometry);
    test_convex_hull<hull_type, Strategy, AreaStrategy>(v, size_original,
        size_hull_closed, expected_area, expected_perimeter, !Clockwise,
        use_only_strategy);
}

template
<
    typename Geometry,
    typename Strategy,
    typename AreaStrategy = typename bg::strategies::area::services::default_strategy
        <
            Geometry
        >::type
>
void test_geometry(std::string const& wkt,
                   std::size_t size_original, std::size_t size_hull_closed,
                   double expected_area, double expected_perimeter = -1.0,
                   bool use_only_strategy = false)
{
    test_geometry_order<Geometry, Strategy, AreaStrategy, true, true>(
        wkt, size_original,
        size_hull_closed, expected_area, expected_perimeter, use_only_strategy
    );
    test_geometry_order<Geometry, Strategy, AreaStrategy, false, true>(
        wkt, size_original,
        size_hull_closed, expected_area, expected_perimeter, use_only_strategy
    );
    test_geometry_order<Geometry, Strategy, AreaStrategy, true, false>(
        wkt, size_original,
        size_hull_closed, expected_area, expected_perimeter, use_only_strategy
    );
    test_geometry_order<Geometry, Strategy, AreaStrategy, false, false>(
        wkt, size_original,
        size_hull_closed, expected_area, expected_perimeter, use_only_strategy
    );
}

template <class SGeometry, class GGeometry>
void test_geometry_sph_geo(std::string const& wkt,
                           std::size_t size_original,
                           std::size_t size_hull_closed,
                           double spherical_expected_area,
                           double geographic_expected_area)
{
    typedef boost::geometry::strategies::convex_hull::spherical<> SphericalStrategy;
    test_geometry<SGeometry, SphericalStrategy>(wkt, size_original,
        size_hull_closed, spherical_expected_area);

    typedef boost::geometry::strategies::convex_hull::geographic<> GeoStrategy;
    test_geometry<GGeometry, GeoStrategy>(wkt, size_original,
        size_hull_closed, geographic_expected_area);
}

template <typename Geometry>
void test_empty_input()
{
    Geometry geometry;
    bg::model::polygon
        <
            typename bg::point_type<Geometry>::type
        > hull;

    bg::convex_hull(geometry, hull);
    BOOST_CHECK_MESSAGE(bg::is_empty(hull), "Output convex hull should be empty" );
}



#endif
