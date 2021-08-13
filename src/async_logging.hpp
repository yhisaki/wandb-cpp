#ifndef WANDBCPP_ASYNC_LOGGING_HPP_
#define WANDBCPP_ASYNC_LOGGING_HPP_

#include <mutex>
#include <thread>

#include "wandbcpp.hpp"

namespace wandbcpp::internal::async {
class AsyncLoggingWorker {
 private:
  std::mutex init_config_mutex_;
  bool is_initialized = false;
  bool init_ready_ = false;
  bool terminal_ = false;

  // log
  std::mutex log_buffer_mutex_;
  std::vector<object::PyDict> log_buffer_;

  // config
  std::mutex config_buffer_mutex_;
  std::vector<object::PyDictItem> config_buffer_;

  // summary
  std::mutex summary_buffer_mutex_;
  std::vector<object::PyDictItem> summary_buffer_;

  // save
  std::mutex file_path_buffer_mutex_;
  std::vector<std::string> file_path_buffer_;

  std::thread logging_worker_thread;

  wandb* wandb_;

 public:
  AsyncLoggingWorker();
  ~AsyncLoggingWorker();
  AsyncLoggingWorker(const AsyncLoggingWorker& other) = delete;
  AsyncLoggingWorker& operator=(const AsyncLoggingWorker& other) = delete;
  void initialize_wandb(const wandb::init_args& ia);
  void worker();

  bool is_log_buffer_empty();
  void append_log(const object::PyDict& log);

  bool is_config_buffer_empty();
  void append_config(const object::PyDictItem& config);

  bool is_summary_buffer_empty();
  void append_summary(const object::PyDictItem& summary);

  bool is_file_path_buffer_empty();
  void append_file_path(const std::string& file_path);

  void finish();
};

}  // namespace wandbcpp::internal::async

#endif