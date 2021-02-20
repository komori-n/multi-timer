# multi-timer

## Getting Started

```sh
git clone --recursive https://github.com/komori-n/multi-timer.git
```

## How to Use

`komori::MultiTimer<Task>` is a simple class to handle multiple timers in a single thread.
`Task` is a type which has `operator()` like `std::function`(default), lambda expression, or
user-defined function objects.

To set a timer, use `MultiTimer::set(task, duration)`.
After `duration` elapsed, `task` will be called from another context.

```cpp
#include <iostream>
#include <cstdlib>
#include <chrono>

#include "komori/multi_timer.hpp"

int main(void){
  komori::MultiTimer<> timer;
  timer.start_processing();

  timer.set([]{ std::cout << "Hoge" << std::endl; }, std::chrono::seconds(3));
  timer.set([]{ std::cout << "Fuga" << std::endl; }, std::chrono::seconds(5));

  while (!timer.empty()) {
    std::this_thread::sleep_for(10ms);
  }

  return EXIT_SUCCESS;
}
```

If you run the above example, "Hoge" will be printed after 3 seconds, and
"Fuga" will be printed after 2 seconds.

## License

Licensed under MIT license
