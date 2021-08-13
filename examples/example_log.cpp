#include <cmath>
#include <iostream>
#include <thread>

#include "wandbcpp.hpp"

namespace chrono = std::chrono;
using namespace std::literals;

int main() {
  wandbcpp::init({
      .project = "example_wandb_cpp"
  });

  int N = 100;

  wandbcpp::add_config({{"N", N}});

  const auto begin = chrono::high_resolution_clock::now();

  for (int i = 0; i < N; i++) {
    double t = M_PI * i / N;
    double x = std::sin(M_PI * i / N);
    double y = std::cos(M_PI * i / N);
    wandbcpp::log({{"t", t}, {"x", x}, {"y", y}});
    std::cout << "i : " << i << std::endl;
    std::this_thread::sleep_for(10.0ms);
  }

  const auto end = chrono::high_resolution_clock::now();
  const auto elapsed_time_us =
      chrono::duration_cast<chrono::microseconds>(end - begin).count();

  wandbcpp::add_summary({{"elapsed_time_us", elapsed_time_us}});
  wandbcpp::save(__FILE__);
}
