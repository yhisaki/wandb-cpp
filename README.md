# WandB-CPP

## Installation

wandb-cpp works by wrapping the [Weight & Biases](https://wandb.ai/site).So, you need to install wandb according to [this procedure](https://docs.wandb.ai/quickstart) first.


## Usage

### Basic Usage

```cpp
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

```

### Save File

```cpp
#include "wandbcpp.hpp"

int main() {
  wandbcpp::init({.project = "example_wandb_cpp", .tags = {"save file"}});
  wandbcpp::save(__FILE__);  // save this source code
  wandbcpp::finish();
}

```

## Build examples

```
mkdir build && cd build
cmake -D BUILD_WANDBCPP_EXE=ON ..
make
```

## Implementation & Performance

This library executes most of its operations in multi-threaded mode.(except for initialization and termination)Therefore, this library should have little impact on the performance of the main processing.

