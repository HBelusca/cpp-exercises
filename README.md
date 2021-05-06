# C++ Exercises

Copyright 2021 Hermès Bélusca-Maïto, under the [GPL-2.0+](https://spdx.org/licenses/GPL-2.0+) license.

Resolutions of some interesting C++ exercises adapted from the suggestions
in [CodeBeauty](https://www.youtube.com/channel/UCl5-BV9aRaeDVohpE4sqJiQ)'s
videos.

The exercises can be compiled with `g++` (GCC/G++) or with `cl` (MSVC),
with support for C++11 or later.

## List

* [quiz.cpp](quiz/quiz.cpp): Quiz application.
```
Usage: quiz.exe <quizfile>
```

* [tasksched.cpp](task/tasksched.cpp): Task list and scheduler.
```
Usage: tasksched.exe [--run] <tasklistfile>

Parameters:
    -r, --run       Optional parameter. When set, schedule the list of tasks.
                    Otherwise, enumerate the list of tasks without scheduling.
    tasklistfile    Text file enumerating the list of tasks.
```
