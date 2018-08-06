// Copyright (c) 2018 Martyn Afford
// Licensed under the MIT licence

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include "poisson_disc_distribution.hpp"

int
main()
{
    constexpr auto width = 80;
    constexpr auto height = 40;

    char map[height][width] = {};

    auto seed = std::chrono::system_clock::now().time_since_epoch().count();

    std::default_random_engine engine{seed};
    std::uniform_real_distribution<float> distribution{0, 1};

    bridson::config conf;
    conf.width = width;
    conf.height = height;
    conf.min_distance = 4.0f;

    bridson::poisson_disc_distribution(
        conf,
        // random
        [&engine, &distribution](float range) {
            return distribution(engine) * range;
        },
        // in_area
        [](bridson::point p) {
            return p.x > 0 && p.x < width && p.y > 0 && p.y < height;
        },
        // output
        [&map](bridson::point p) {
            map[static_cast<int>(p.y)][static_cast<int>(p.x)] = '.';
        });

    for (auto& row : map) {
        for (auto& cell : row) {
            std::cout << (cell ? cell : ' ');
        }

        std::cout << '\n';
    }

    return EXIT_SUCCESS;
}
