#include <iostream>
#include <cstdlib>
#include <chrono>
#include <future>

#include "komori/multi_timer.hpp"
#include "komori/unique_function.hpp"

using std::chrono::operator"" s;


int main(void){
  komori::MultiTimer<komori::unique_function<void(void)>> timer;
  timer.start_processing();

  std::promise<int> promise_1;
  std::promise<int> promise_2;
  std::promise<int> promise_3;
  auto future_1 = promise_1.get_future();
  auto future_2 = promise_2.get_future();
  auto future_3 = promise_3.get_future();

  timer.set([promise=std::move(promise_1)] (void) mutable {
    promise.set_value(334);
  }, 4s);
  timer.set([promise=std::move(promise_2)] (void) mutable {
    promise.set_value(264);
  }, 3s);
  timer.set([promise=std::move(promise_3)] (void) mutable {
    promise.set_value(227);
  }, 2s);

  std::cout << future_3.get() << std::endl;
  std::cout << future_2.get() << std::endl;
  std::cout << future_1.get() << std::endl;

  return EXIT_SUCCESS;
}

