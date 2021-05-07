/*
 * PROJECT:     C++ Task Scheduler
 * LICENSE:     GPL-2.0+ (https://spdx.org/licenses/GPL-2.0+)
 * COPYRIGHT:   Copyright 2021 Hermès Bélusca-Maïto
 *
 * PURPOSE:     A task scheduler example, demonstrating different aspects
 *              and features of the C++ language.
 *              Based on the "Queue and FIFO/FCFS explained in 10 minutes"
 *              video challenge https://youtu.be/jaK4pn1jXTo by "CodeBeauty".
 *
 * Compilation:
 * - G++:   g++ tasksched.cpp -o tasksched.exe
 * - MSVC:  cl /EHsc tasksched.cpp /Fe:tasksched.exe
 *
 * Usage: tasksched.exe <tasklistfile>
 *
 * Parameters:
 *     tasklistfile    Text file enumerating the list of tasks.
 *
 * A "task list file" could represent for example a daily schedule
 * (more practical when this schedule does not change much).
 * One task per line.
 */

/*
 * This program illustrates some of the concepts viewed in different videos,
 * using C++ and the STL library:
 * - Text file reading: https://youtu.be/EaHFhms_Shw
 * - STL queues: https://youtu.be/jaK4pn1jXTo
 */

#include <iostream>     // For IO streams.
#include <fstream>      // For file streams.
#include <queue>        // For std::queue<>
#include <string>       // For std::string

// using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char** argv)
{
    /* Stop now if we don't have any file */
    if (argc <= 1)
    {
        cout << "Usage: " << argv[0] << " <tasklistfile>\n\n"
             << "Parameters:\n"
                "    tasklistfile    Text file enumerating the list of tasks." << endl;
        return -1;
    }

    /* Try to open the task list file for input */
    std::ifstream inFile;
    inFile.open(argv[1], std::ios::in);
    if (!inFile.is_open())
    {
        cerr << "Could not open task list file '" << argv[1] << "'" << endl;
        return -1;
    }

    /*
     * Parse the tasks from the file.
     */
    std::queue<std::string> tasks;
    while (true)
    {
        std::string line;

        /* Try to read an entire line representing a task */
        std::getline(inFile, line);

        /* Stop now if we reached the end of the file */
        if (inFile.eof())
            break;

        /* If the line is empty, skip this entry */
        if (line.empty())
            continue;

        /* Append this new task */
        tasks.push(line);
    }

    /* We are done with the task file */
    inFile.close();

    /* Print the header */
    cout << "==== Tasks for Today ====\n" << endl;

    /* Save whether we originally had tasks to do (used for later) */
    bool bHadTasks = !tasks.empty();

    /* Enumerate the tasks we need to do */
    while (!tasks.empty())
    {
        /* Retrieve the next task and pop it */
        cout << tasks.front() << endl;
        tasks.pop();
    }
    if (bHadTasks)
        cout << endl;

    /* And we are done! */
    cout << (bHadTasks ? "You have finished all your tasks, congratulations! You've earned it!"
                       : "Nothing to do today! Relax & enjoy!") << endl;
    return 0;
}
