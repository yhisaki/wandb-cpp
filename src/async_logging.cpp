#include "src/async_logging.hpp"

namespace wandbcpp::internal::async {
using namespace std::literals;

AsyncLoggingWorker::AsyncLoggingWorker()
    : is_initialized(false),
      init_ready_(false),
      terminal_(false),
      logging_worker_thread(std::thread(&AsyncLoggingWorker::worker, this)) {}

AsyncLoggingWorker::~AsyncLoggingWorker() {
  finish();
  logging_worker_thread.join();
  delete wandb_;
  Py_FinalizeEx();
}

void AsyncLoggingWorker::initialize_wandb(
    const std::string& project, const std::string& name,
    const std::vector<std::string>& tags) {
  Py_Initialize();
  wandb_ = new wandb();
  wandb_->init(project, name, tags);
  is_initialized = true;
}

void AsyncLoggingWorker::worker() {
  while (true) {
    std::this_thread::sleep_for(0.2s);
    if (is_initialized) break;
  }
  while (true) {
    if (!is_log_buffer_empty()) {
      std::vector<object::PyDict> log_buffer_moved_;
      {
        std::lock_guard<std::mutex> lock(log_buffer_mutex_);
        log_buffer_moved_ = std::move(log_buffer_);
      }
      for (auto&& log : log_buffer_moved_) {
        wandb_->log(log);
      }
    }
    if (!is_config_buffer_empty()) {
      std::vector<object::PyDictItem> config_buffer_moved_;
      {
        std::lock_guard<std::mutex> lock(config_buffer_mutex_);
        config_buffer_moved_ = std::move(config_buffer_);
      }
      for (auto&& config : config_buffer_moved_) {
        wandb_->add_config(config);
      }
    }
    if (!is_summary_buffer_empty()) {
      std::vector<object::PyDictItem> summary_buffer_moved_;
      {
        std::lock_guard<std::mutex> lock(summary_buffer_mutex_);
        summary_buffer_moved_ = std::move(summary_buffer_);
      }
      for (auto&& summary : summary_buffer_moved_) {
        wandb_->add_summary(summary);
      }
    }
    if (!is_file_path_buffer_empty()) {
      std::vector<std::string> file_path_buffer_moved_;
      {
        std::lock_guard<std::mutex> lock(file_path_buffer_mutex_);
        file_path_buffer_moved_ = std::move(file_path_buffer_);
      }
      for (auto& file_path : file_path_buffer_moved_) {
        wandb_->save(file_path);
      }
    }
    if (terminal_ &&                 //
        is_log_buffer_empty() &&     //
        is_config_buffer_empty() &&  //
        is_summary_buffer_empty() &&  //
        is_file_path_buffer_empty())
      break;
    std::this_thread::sleep_for(0.2s);
  }
}

bool AsyncLoggingWorker::is_log_buffer_empty() {
  std::lock_guard<std::mutex> lock(log_buffer_mutex_);
  return log_buffer_.empty();
}

void AsyncLoggingWorker::append_log(const object::PyDict& log) {
  std::lock_guard<std::mutex> lock(log_buffer_mutex_);
  log_buffer_.emplace_back(log);
}

bool AsyncLoggingWorker::is_config_buffer_empty() {
  std::lock_guard<std::mutex> lock(config_buffer_mutex_);
  return config_buffer_.empty();
}

void AsyncLoggingWorker::append_config(const object::PyDictItem& config) {
  std::lock_guard<std::mutex> lock(config_buffer_mutex_);
  config_buffer_.emplace_back(config);
}

bool AsyncLoggingWorker::is_summary_buffer_empty() {
  std::lock_guard<std::mutex> lock(summary_buffer_mutex_);
  return summary_buffer_.empty();
}

void AsyncLoggingWorker::append_summary(const object::PyDictItem& summary) {
  std::lock_guard<std::mutex> lock(summary_buffer_mutex_);
  summary_buffer_.emplace_back(summary);
}

bool AsyncLoggingWorker::is_file_path_buffer_empty() {
  std::lock_guard<std::mutex> lock(file_path_buffer_mutex_);
  return file_path_buffer_.empty();
}
void AsyncLoggingWorker::append_file_path(const std::string& file_path) {
  std::lock_guard<std::mutex> lock(file_path_buffer_mutex_);
  file_path_buffer_.emplace_back(file_path);
}

void AsyncLoggingWorker::finish() { terminal_ = true; }

}  // namespace wandbcpp::internal::async