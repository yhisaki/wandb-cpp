#include <cmath>
#include <iostream>
#include <thread>

#include "wandbcpp.hpp"

namespace chrono = std::chrono;
using namespace std::literals;

int main() {
  wandbcpp::init("test_wandb_cpp");

  int N = 100;

  wandbcpp::add_config({{"N", N}});

  const auto begin = chrono::high_resolution_clock::now();

  for (int i = 0; i < N; i++) {
    double t = M_PI * i / N;
    double x = std::sin(M_PI * i / N);
    double y = std::cos(M_PI * i / N);
    double x_ref = std::sin(M_PI * i / N) + 0.1;
    double y_ref = std::cos(M_PI * i / N) + 0.1;
    wandbcpp::log({{"t", t}, {"x", x}, {"y", y}, {"x_ref", x_ref}, {"y_ref", y_ref}});
    std::cout << "i : " << i << std::endl;
    std::this_thread::sleep_for(10.0ms);
  }

  wandbcpp::save(__FILE__);

  const auto end = chrono::high_resolution_clock::now();
  const auto elapsed_time_us =
      chrono::duration_cast<chrono::microseconds>(end - begin).count();

  std::cout << double(elapsed_time_us) / double(N) * 1E-6 << "[s]\n"
            << std::endl;
}
