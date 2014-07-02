// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2014 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2014 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2014 Mateusz Loskot, London, UK.
// Copyright (c) 2013-2014 Adam Wulkiewicz, Lodz, Poland.
// Copyright (c) 2014 Samuel Debionne, Grenoble, France.

// This file was modified by Oracle on 2014.
// Modifications copyright (c) 2014, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_STRATEGIES_DISTANCE_RESULT_HPP
#define BOOST_GEOMETRY_STRATEGIES_DISTANCE_RESULT_HPP

#include <boost/mpl/always.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/vector.hpp>

#include <boost/variant/variant_fwd.hpp>

#include <boost/geometry/core/point_type.hpp>

#include <boost/geometry/strategies/default_strategy.hpp>
#include <boost/geometry/strategies/distance.hpp>

#include <boost/geometry/util/compress_variant.hpp>
#include <boost/geometry/util/transform_variant.hpp>
#include <boost/geometry/util/combine_if.hpp>

#include <boost/geometry/algorithms/detail/distance/default_strategies.hpp>


namespace boost { namespace geometry
{


namespace resolve_strategy { namespace result_of
{

template <typename Geometry1, typename Geometry2, typename Strategy>
struct distance
    : strategy::distance::services::return_type
        <
            Strategy,
            typename point_type<Geometry1>::type,
            typename point_type<Geometry2>::type
        >
{};

template <typename Geometry1, typename Geometry2>
struct distance<Geometry1, Geometry2, default_strategy>
    : distance
        <
            Geometry1,
            Geometry2,
            typename detail::distance::default_strategy
                <
                    Geometry1, Geometry2
                >::type
        >
{};

}} // namespace resolve_strategy::result_of


namespace resolve_variant { namespace result_of
{

template <typename Geometry1, typename Geometry2, typename Strategy>
struct distance
    : resolve_strategy::result_of::distance
        <
            Geometry1,
            Geometry2,
            Strategy
        >
{};


template <typename Geometry1, BOOST_VARIANT_ENUM_PARAMS(typename T), typename Strategy>
struct distance<Geometry1, variant<BOOST_VARIANT_ENUM_PARAMS(T)>, Strategy>
{
    // A set of all variant type combinations that are compatible and implemented
    typedef typename util::combine_if<
        typename mpl::vector1<Geometry1>,
        typename variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types,
        // Here we want should remove most of the combinations that are not valid
        // mostly to limit the size of the resulting MPL set.
        // But is_implementedn is not ready for prime time
        //
        // util::is_implemented2<mpl::_1, mpl::_2, dispatch::distance<mpl::_1, mpl::_2> >
        mpl::always<mpl::true_>
    >::type possible_input_types;

    // The (possibly variant) result type resulting from these combinations
    typedef typename compress_variant<
        typename transform_variant<
            possible_input_types,
            resolve_strategy::result_of::distance<
                point_type<mpl::first<mpl::_> >,
                point_type<mpl::second<mpl::_> >,
                Strategy
            >,
            mpl::back_inserter<mpl::vector0<> >
        >::type
    >::type type;
};


// Distance arguments are commutative
template <BOOST_VARIANT_ENUM_PARAMS(typename T), typename Geometry2, typename Strategy>
struct distance<variant<BOOST_VARIANT_ENUM_PARAMS(T)>, Geometry2, Strategy>
    : public distance<Geometry2, variant<BOOST_VARIANT_ENUM_PARAMS(T)>, Strategy>
{};


template <BOOST_VARIANT_ENUM_PARAMS(typename T), typename Strategy>
struct distance<variant<BOOST_VARIANT_ENUM_PARAMS(T)>, variant<BOOST_VARIANT_ENUM_PARAMS(T)>, Strategy>
{
    // A set of all variant type combinations that are compatible and implemented
    typedef typename util::combine_if
        <
            typename variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types,
            typename variant<BOOST_VARIANT_ENUM_PARAMS(T)>::types,
            // Here we want to try to remove most of the combinations that are not valid
            // mostly to limit the size of the resulting MPL vector.
            // But is_implementedn is not ready for prime time
            //
            // util::is_implemented2<mpl::_1, mpl::_2, dispatch::distance<mpl::_1, mpl::_2> >
            mpl::always<mpl::true_>
        >::type possible_input_types;

    // The (possibly variant) result type resulting from these combinations
    typedef typename compress_variant<
        typename transform_variant<
            possible_input_types,
            resolve_strategy::result_of::distance<
                point_type<mpl::first<mpl::_> >,
                point_type<mpl::second<mpl::_> >,
                Strategy
            >,
            mpl::back_inserter<mpl::vector0<> >
        >::type
    >::type type;
};

}} // namespace resolve_variant::result_of


/*!
\brief Meta-function defining return type of distance function
\ingroup distance
\note The strategy defines the return-type (so this situation is different
    from length, where distance is sqr/sqrt, but length always squared)
 */
template
<
    typename Geometry1,
    typename Geometry2 = Geometry1,
    typename Strategy = void
>
struct distance_result
    : resolve_variant::result_of::distance<Geometry1, Geometry2, Strategy>
{};


template <typename Geometry1, typename Geometry2>
struct distance_result<Geometry1, Geometry2, void>
    : distance_result<Geometry1, Geometry2, default_strategy>
{};


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_STRATEGIES_DISTANCE_RESULT_HPP
