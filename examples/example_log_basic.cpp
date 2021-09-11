#include <cmath>
#include <iostream>

#include "wandbcpp.hpp"

int main() {
  wandbcpp::init({.project = "example_wandb_cpp"});

  int N = 100;

  wandbcpp::add_config({{"N", N}, {"mode", "basic"}});

  for (int i = 0; i < N; i++) {
    double t = M_PI * i / N;
    double x = std::sin(M_PI * i / N);
    double y = std::cos(M_PI * i / N);
    wandbcpp::log({{"t", t}, {"x", x}, {"y", y}});
    std::cout << "i : " << i << std::endl;
  }

  wandbcpp::finish();
}
