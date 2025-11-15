#include "math/statistics.h"
#include "math/waves.h"
#include "wave/writer.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <math/qnumber.h>
#include <numbers>
#include <print>
#include <ranges>
#include <span>
#include <vector>

TEST_CASE("cordic::sine() to std::sin() correlation")
{
    std::vector<float> radians(10000);
    bit::linear_space(std::begin(radians),
                      std::end(radians),
                      -2 * std::numbers::pi,
                      2 * std::numbers::pi);
    auto std_sine =
        std::views::transform([](float angle) { return std::sin(angle); });
    auto cordic_sine = std::views::transform([](float angle) {
        auto argument = bit::qs<5, 25>{angle};
        return bit::cordic::sine<31>(argument).as<float>();
    });
    auto correlation =
        bit::pearson_correlation(radians | std_sine, radians | cordic_sine);
    std::println("Correlation: {}", correlation.value());
    CHECK(correlation.value() == Catch::Approx(1.f).epsilon(1e-5));
}

TEST_CASE("cordic::sine() to wav file")
{
    auto duration           = 10.f;                 // s
    auto wave_frequency     = 440.f;                // Hz
    auto period             = 1.f / wave_frequency; // s
    auto sampling_frequency = 44'100u;              // Hz
    auto number_of_samples  = static_cast<int>(duration * sampling_frequency);
    std::vector<int32_t> samples(number_of_samples);

    bit::wave::header header{1, sampling_frequency, 32};
    assert(validate_header(header));
    bit::wave::writer wave_writer{header, "cordic_sine.wav"};

    bit::oscilator sine{
        [](float phase) {
            auto phase_qformatted = bit::qs<10, 12>{phase};
            return bit::cordic::sine<7>(phase_qformatted).raw() * (1 << 24);
        },
        wave_frequency,
        sampling_frequency};

    sine.next(std::span{samples});
    wave_writer.write(std::span{samples});
}
