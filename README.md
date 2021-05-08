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
Simple Task Scheduler v1.0
Copyright 2021 Hermès Bélusca-Maïto
Under GPL-2.0+ license (https://spdx.org/licenses/GPL-2.0+)

Usage:
    tasksched.exe [--run] tasklistfile
    command-name | tasksched.exe [--run]
    tasksched.exe [--run] < tasklistfile

    command-name    Specifies a command whose output is formatted similarly
                    to a task list file, and whose tasks will be scheduled.

Parameters:
    -r, --run       Optional parameter. When set, schedule the list of tasks.
                    Otherwise, enumerate the list of tasks without scheduling.

    tasklistfile    Text file enumerating the list of tasks. It can either be
                    passed as an option, or be redirected to the STDIN.

    -?, --help      Displays this help message.

Each line in the task list file describes a single task, and has
the following format:
    time <whitespace> Task_description
where:
- 'time' is in hour:minutes (HH:MM) format. This is optional.
- 'Task_description' is a one-line string describing the task.
Whitespace is trimmed around the task description.
```
