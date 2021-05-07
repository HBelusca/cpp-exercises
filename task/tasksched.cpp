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

/* TEST MODE: Enable to compile and run this program in test mode. */
// #define TEST_MODE


#define _CRT_SECURE_NO_WARNINGS
#include <ctime>        // For C++ time_t wrappers around "time.h".
#include <iomanip>      // Used here for time parsing routines.
//----
#include <iostream>     // For IO streams.
#include <fstream>      // For file streams.
#include <queue>        // For std::queue<>
#include <string>       // For std::string
#ifdef TEST_MODE
#include <sstream>      // For string streams.
#endif


#ifdef TEST_MODE
const std::string testSchedule =
"  \t  00:00  \t  Midnight  \t  \n" // Leading/trailing whitespace (-> trim)
"\n\n"                      // Extra newlines (-> ignore)
"   \t    \n"               // Only whitespace (-> ignore)
"6:00  \t  Wake up\n"
"7:00    Breakfast\n"
"7:30    Go to work\n"
"\n\n\n"                    // Extra newlines (-> ignore)
"8:00    Do work stuff\n"
"12:00   Lunch break\n"
"13:00   Back to work stuff\n"
"14:30   Meeting\n"
"16:00   Music lesson\n"
"\n\n";
#endif


// using std::cin;
using std::cout;
using std::cerr;
using std::endl;

#ifndef TEST_MODE
static void
Usage(const std::string& exePath)
{
    /* Extract the file name only (after the last path separator) */
    size_t pos;
    std::string exeName(exePath);
#ifdef _WIN32 // Win32 supports both backward and forward slashes.
    pos = exeName.find_last_of("\\/");
#else // Other platforms: most likely just forward slash.
    pos = exeName.rfind('/');
#endif
    exeName = exeName.substr(pos != std::string::npos ? pos + 1 : 0);

    cout << "Usage: " << exeName << " <tasklistfile>\n\n"
         << "Parameters:\n"
            "    tasklistfile    Text file enumerating the list of tasks." << endl;
}
#endif

int main(int argc, char** argv)
{
#ifdef TEST_MODE

    std::istringstream inFile(testSchedule);

#else

    /* Stop now if we don't have any file */
    if (argc <= 1)
    {
        Usage(argv[0]);
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

#endif

    /* Set the current user's locale (for nicely displaying dates...) */
    cout.imbue(std::locale(""));

    /* Time support */
    time_t t_today = time(nullptr);

    /*
     * Parse the tasks from the file.
     */
    std::queue<std::string> tasks;
    while (true)
    {
        size_t pos;
        std::string line;

        /* Try to read an entire line representing a task */
        std::getline(inFile, line);

        /* Stop now if we reached the end of the file */
        if (inFile.eof())
            break;

        constexpr const char* WHITESPACE = " \t\f\v\n\r";

        /* Trim leading and trailing whitespace */
        pos = line.find_first_not_of(WHITESPACE);
        line = line.substr(pos != std::string::npos ? pos : line.length());
        pos = line.find_last_not_of(WHITESPACE);
        line = line.substr(0, pos != std::string::npos ? pos + 1 : 0);

        /* If the line is empty, skip this entry */
        if (line.empty())
            continue;

        /* Append this new task */
        tasks.push(line);
    }

#ifndef TEST_MODE
    /* We are done with the task file */
    inFile.close();
#endif

    /* Print the header */
    cout << "==== Tasks for Today, "
         << std::put_time(localtime(&t_today), "%A %x")
         << " ====\n" << endl;

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
