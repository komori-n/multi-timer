#include <iostream>
#include <cstdlib>
#include <chrono>

#include "komori/multi_timer.hpp"

using std::chrono::operator"" ms;
using std::chrono::operator"" s;


int main(void){
  komori::MultiTimer<> timer;
  timer.start_processing();

  timer.set([]{ std::cout << "3s" << std::endl; }, 3s);
  timer.set([]{ std::cout << "6s" << std::endl; }, 6s);

  timer.set([&]{
    std::cout << "2s" << std::endl;
    timer.set([&]{
      std::cout << "2s+2s" << std::endl;
      timer.set([&]{
        std::cout << "2s+2s+2s" << std::endl;
      }, 2s);
    }, 2s);
  }, 2s);

  while (!timer.empty()) {
    std::this_thread::sleep_for(10ms);
  }

  return EXIT_SUCCESS;
}
