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
  wandbcpp::init({.project = "example_wandb_cpp", .tags = {"basic"}});

  int N = 100;

  wandbcpp::add_config({{"N", N}, {"mode", "abc"}});

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

- [result](https://wandb.ai/hisaki/example_wandb_cpp/runs/2runz37w)

### Save File

```cpp
#include "wandbcpp.hpp"

int main() {
  wandbcpp::init({.project = "example_wandb_cpp", .tags = {"save file"}});
  wandbcpp::save(__FILE__);  // save this source code
  wandbcpp::finish();
}
```

- [result](https://wandb.ai/hisaki/example_wandb_cpp/runs/1g5qxcfh)

### wandb.Table

- use `add_data`

  ```cpp
  #include <cmath>

  #include "wandbcpp.hpp"

  int main() {
    wandbcpp::init({.project = "example_wandb_cpp", .tags = {"table"}});
    int I = 10;
    int J = 100;

    for (int i = 0; i < I; i++) {
      wandbcpp::Table table({"x", "y"});
      for (int j = 0; j < J; j++) {
        double x = (i + 1) * std::sin(M_PI * j / J);
        double y = (i + 1) * std::cos(M_PI * j / J);
        table.add_data({x, y});
      }
      wandbcpp::log({{"mytable", table}});
    }
    wandbcpp::finish();
  }
  ```

  - [result](https://wandb.ai/hisaki/example_wandb_cpp/runs/2m6djvut)

- use `add_column`

  ```cpp
  #include "wandbcpp.hpp"

  int main() {
    wandbcpp::init({.project = "example_wandb_cpp", .tags = {"table"}});
    wandbcpp::Table table;
    std::vector<double> x = {1.0, 2.0, 3.0};
    std::vector<int> y = {4, 5, 6};
    table.add_column("x", {x.begin(), x.end()});
    table.add_column("y", {y.begin(), y.end()});
    wandbcpp::log({{"table", table}});
    wandbcpp::finish();
  }
  ```

  - [result](https://wandb.ai/hisaki/example_wandb_cpp/runs/3ijm1cne)

- passing data in the constructor

  ```cpp
  #include "wandbcpp.hpp"

  using namespace std::literals;

  int main() {
    wandbcpp::init({.project = "example_wandb_cpp", .tags = {"table"}});
    int I = 10;
    for (int i = 0; i < I; i++) {
      wandbcpp::Table table({"x", "y"}, {{"poyo", i}, {"hoge", i + 1}});
      wandbcpp::log({{"table", table}});
    }
    wandbcpp::finish();
  }
  ```

  - [result](https://wandb.ai/hisaki/example_wandb_cpp/runs/ytswbgny)

## Build examples

```
mkdir build && cd build
cmake -D BUILD_WANDBCPP_EXE=ON ..
make
```

## Implementation & Performance

This library executes most of its operations in multi-threaded mode.(except for initialization and termination)Therefore, this library should have little impact on the performance of the main processing.

