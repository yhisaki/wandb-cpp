# WandB-CPP

## Usage

```cpp
#include <cmath>

#include "wandbcpp.hpp"

int main() {
  wandbcpp::init("minimal_example");

  int N = 100;

  wandbcpp::add_config({{"N", N}, {"mode", "minimal"}});

  for (int i = 0; i < N; i++) {
    double t = M_PI * i / N;
    double x = std::sin(M_PI * i / N);
    double y = std::cos(M_PI * i / N);
    wandbcpp::log({{"t", t}, {"x", x}, {"y", y}});
    std::cout << "i : " << i << std::endl;
  }
}
```

## build examples

```
mkdir build && cd build
cmake -D BUILD_WANDBCPP_EXE=ON ..
make
```
