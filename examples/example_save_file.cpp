#include "wandbcpp.hpp"

int main() {
  wandbcpp::init({.project = "example_wandb_cpp", .tags = {"save file"}});
  wandbcpp::save(__FILE__);  // save this source code
  wandbcpp::finish();
}
