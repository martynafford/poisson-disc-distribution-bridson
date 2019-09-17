// Copyright (c) 2018 Martyn Afford
// Licensed under the MIT licence

#ifndef BRIDSON_POISSON_DISC_DISTRIBUTION_HPP
#define BRIDSON_POISSON_DISC_DISTRIBUTION_HPP

#include <cmath>
#include <limits>
#include <stack>
#include <vector>

namespace bridson {

constexpr static auto infinity = std::numeric_limits<float>::infinity();

struct point {
    float x = infinity;
    float y = infinity;
};

// A configuration structure to customise the poisson_disc_distribution
// algorithm below.
//
//   width, height - Defines the range of x as (0, width] and the range
//                   of y as (0, height].
//
//   min_distance  - The smallest distance allowed between two points. Also,
//                   points will never be further apart than twice this
//                   distance.
//
//   max_attempts  - The algorithm stochastically attempts to place a new point
//                   around a current point. This number limits the number of
//                   attempts per point. A lower number will speed up the
//                   algorithm but at some cost, possibly significant, to the
//                   result's aesthetics.
//
//   start         - An optional parameter. If set to anything other than
//                   point's default values (infinity, infinity) the algorithm
//                   will start from this point. Otherwise a point is chosen
//                   randomly. Expected to be within the region defined by
//                   width and height.
struct config {
    float width = 1.0f;
    float height = 1.0f;
    float min_distance = 0.05f;
    int max_attempts = 30;
    point start;
};

// This implements the algorithm described in 'Fast Poisson Disk Sampling in
// Arbitrary Dimensions' by Robert Bridson. This produces a random set of
// points such that no two points are closer than conf.min_distance apart or
// further apart than twice that distance.
//
// Parameters
//
//   conf    - The configuration, as detailed above.
//
//   random  - A callback of the form float(float limit) that returns a random
//             value ranging from 0 (inclusive) to limit (exclusive).
//
//   in_area - A callback of the form bool(point) that returns whether a point
//             is within a valid area. This can be used to create shapes other
//             than rectangles. Points can't be outside of the defined limits of
//             the width and height specified. See the notes section for more.
//
//   output  - A callback of the form void(point). All points that are part of
//             the final Poisson disc distribution are passed here.
//
// Notes
//
//   The time complexity is O(n) where n is the number of points.
//
//   The in_area callback must prevent points from leaving the region defined by
//   width and height (i.e., 0 <= x < width and 0 <= y < height). If this is
//   not done invalid memory accesses will occur and most likely a segmentation
//   fault.
template <typename T, typename T2, typename T3>
void
poisson_disc_distribution(config conf, T&& random, T2&& in_area, T3&& output)
{
    float cell_size = conf.min_distance / std::sqrt(2);
    int grid_width = std::ceil(conf.width / cell_size);
    int grid_height = std::ceil(conf.height / cell_size);

    std::vector<point> grid(grid_width * grid_height);
    std::stack<point> process;

    auto squared_distance = [](const point& a, const point& b) {
        auto delta_x = a.x - b.x;
        auto delta_y = a.y - b.y;

        return delta_x * delta_x + delta_y * delta_y;
    };

    auto point_around = [&conf, &random](point p) {
        auto radius = conf.min_distance * std::sqrt(random(3) + 1);
        auto angle = random(2 * M_PI);

        p.x += std::cos(angle) * radius;
        p.y += std::sin(angle) * radius;

        return p;
    };

    auto set = [cell_size, grid_width, &grid](const point& p) {
        int x = p.x / cell_size;
        int y = p.y / cell_size;
        grid[y * grid_width + x] = p;
    };

    auto add = [&process, &output, &set](const point& p) {
        process.push(p);
        output(p);
        set(p);
    };

    auto point_too_close = [&](const point& p) {
        int x_index = std::floor(p.x / cell_size);
        int y_index = std::floor(p.y / cell_size);

        if (grid[y_index * grid_width + x_index].x != infinity) {
            return true;
        }

        auto min_dist_squared = conf.min_distance * conf.min_distance;
        auto min_x = std::max(x_index - 2, 0);
        auto min_y = std::max(y_index - 2, 0);
        auto max_x = std::min(x_index + 2, grid_width - 1);
        auto max_y = std::min(y_index + 2, grid_height - 1);

        for (auto y = min_y; y <= max_y; ++y) {
            for (auto x = min_x; x <= max_x; ++x) {
                auto point = grid[y * grid_width + x];
                auto exists = point.x != infinity;

                if (exists && squared_distance(p, point) < min_dist_squared) {
                    return true;
                }
            }
        }

        return false;
    };

    if (conf.start.x == infinity) {
        do {
            conf.start.x = random(conf.width);
            conf.start.y = random(conf.height);
        } while (!in_area(conf.start));
    }

    add(conf.start);

    while (!process.empty()) {
        auto point = process.top();
        process.pop();

        for (int i = 0; i != conf.max_attempts; ++i) {
            auto p = point_around(point);

            if (in_area(p) && !point_too_close(p)) {
                add(p);
            }
        }
    }
}

} // bridson namespace

#endif /* BRIDSON_POISSON_DISC_DISTRIBUTION_HPP */
