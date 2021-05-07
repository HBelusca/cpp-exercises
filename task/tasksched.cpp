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
 * NOTE: Uses C++11 features.
 *
 * Compilation:
 * - G++:   g++ tasksched.cpp -o tasksched.exe
 * - MSVC:  cl /EHsc tasksched.cpp /Fe:tasksched.exe
 *
 * Usage: tasksched.exe [--run] <tasklistfile>
 *
 * Parameters:
 *     -r, --run       Optional parameter. When set, schedule the list of tasks.
 *                     Otherwise, enumerate the list of tasks without scheduling.
 *     tasklistfile    Text file enumerating the list of tasks.
 *
 * A "task list file" could represent for example a daily schedule
 * (more practical when this schedule does not change much).
 * One task per line:
 *   time <whitespace> Task_description
 * where:
 * - 'time' is in hour:minutes (HH:MM) format. This is optional.
 * - 'Task_description' is a one-line string describing the task.
 *
----------------------------
6:00    Wake up
6:30    Shower
7:00    Breakfast
7:30    Go to work
8:00    Do work stuff
12:00   Lunch break
13:00   Back to work stuff
14:30   Meeting
16:00   Music lesson
 */

/*
 * This program illustrates some of the concepts viewed in different videos,
 * using C++ and the STL library:
 * - Structures / classes: https://youtu.be/iVLQeWbgbXs etc.
 * - Templates: https://youtu.be/qrJjFN4Igfw
 * - Text file reading: https://youtu.be/EaHFhms_Shw
 * - STL queues: https://youtu.be/jaK4pn1jXTo
 *
 * Other concepts:
 * - time_t, tm;
 * - Overloading comparison and injection operators.
 */

/* "--run": Enable to support task scheduling (requires C++11). */
#define TASK_RUN_SCHEDULE

/* TEST MODE: Enable to compile and run this program in test mode. */
// #define TEST_MODE


#define _CRT_SECURE_NO_WARNINGS
#include <ctime>        // For C++ time_t wrappers around "time.h".
#include <iomanip>      // Used here for time parsing routines.
/*
 * HACK around braindead G++ -- Only usable for *nix!
 * While the standard explicitly says that leading zeroes are optional
 * in front of the digits in %H and %M formatting, G++ STL mandates them
 * for std::get_time(), and fails parsing otherwise!
 *
 * See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=45896 (bug since 2010)
 * and https://gcc.gnu.org/bugzilla/show_bug.cgi?id=71556 (bug since 2016)
 * for more details.
 */
#ifndef _WIN32
#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
#define GCC_TIME_WORKAROUND
#include <sys/time.h>
#endif
#endif
//----
#ifdef TASK_RUN_SCHEDULE
#include <thread>       // For std::this_thread::sleep_until()
#include <chrono>       // For std::chrono::system_clock
#endif
//----
#include <io.h>         // For _isatty()
#include <iostream>     // For IO streams.
#include <fstream>      // For file streams.
//----
#include <queue>        // For std::queue<>
#include <algorithm>    // For std::sort()
//----
#include <cstring>      // For strcmp()
#include <string>       // For std::string
#include <sstream>      // For string streams.
//----
#include <stdexcept>    // For std::runtime_error


#ifdef TEST_MODE
const std::string testSchedule =
"  \t  00:00  \t  Midnight  \t  \n" // Leading/trailing whitespace (-> trim)
"Generic_Task0\n"           // No time task, no whitespaces
"\n\n"                      // Extra newlines (-> ignore)
"   \t    \n"               // Only whitespace (-> ignore)
"6:00  \t  Wake up\n"
"6:30\n"                    // No description (-> ignore)
"    Generic Task 1\n"      // No time task
"7:00    Breakfast\n"
"7:00    Bath\n"            // Colliding task
"7:30    Go to work\n"
"\n\n\n"                    // Extra newlines (-> ignore)
"8:00    Do work stuff\n"
"14:30   Meeting\n"         // Unordered item
"12:00   Lunch break\n"
"    Generic Task 2\n"      // No time task
"16:00   Music lesson\n"    // Unordered item
"13:00   Back to work stuff\n"
"\n\n";
#endif

/** @brief  Represents one task. */
class CTask
{
public:

/* Constructors */
#if 0
    CTask(const std::string timeStr,
          const std::string& description) :
    m_Time(timeStr),
    m_Description(description)
    {};
#endif

    CTask(const time_t time,
          const std::string& description) :
    m_Time(time),
    m_Description(description)
    {};

    CTask(tm* const time,
          const std::string& description) :
    CTask(mktime(time), description)
    {};

    CTask(const std::string& description) :
    CTask(time_t(-1), description)
    {};

/* Destructors */
    virtual ~CTask() {}; // = default;

/* Getters / Setters */
    time_t time() const { return m_Time; }
    void time(const time_t time) { m_Time = time; }
    void time(tm* const time) { m_Time = mktime(time); }

    const std::string& description() const { return m_Description; }
    void description(const std::string& description) { m_Description = description; }

/*
 * Comparison operators - Used for comparing tasks (e.g. task queue sorting).
 * NOTE: The C++20 fancy way is to define an auto operator<=>(const CTask&) const;
 */
    friend bool operator<(const CTask& task1, const CTask& task2);
    friend bool operator>(const CTask& task1, const CTask& task2);
    friend bool operator<=(const CTask& task1, const CTask& task2);
    friend bool operator>=(const CTask& task1, const CTask& task2);
    friend bool operator==(const CTask& task1, const CTask& task2) /*throw()*/;
    friend bool operator!=(const CTask& task1, const CTask& task2) /*throw()*/;

    /**
     * @brief   Overloaded injection operator.
     *          Used as a simple display function for the CTask.
     *          Usage example:
     *             cout << task;
     *          Displays the time and description of the task.
     */
    friend std::ostream& operator<<(std::ostream& os, const CTask& task);

protected:
    time_t m_Time; // Timestamp. Is == -1 if no time associated.
    std::string m_Description; // Task description.
};

/*
 * Comparison operators
 */

/** @brief  Return true if task1.m_Time < task2.m_Time, and false otherwise. */
bool operator<(const CTask& task1, const CTask& task2)
{
    /* Particular case for non-timed tasks */
    if (task1.m_Time == -1)
        return true;

    /* General case */
    return (task1.m_Time < task2.m_Time);
}

bool operator>(const CTask& task1, const CTask& task2)
{
    return (task2 < task1);
}

bool operator<=(const CTask& task1, const CTask& task2)
{
    return !(task1 > task2);
}

bool operator>=(const CTask& task1, const CTask& task2)
{
    return !(task1 < task2);
}

/**
 * @brief   Return true if task1.m_Time == task2.m_Time, and false otherwise.
 *          Throws an exception if the times are same but descriptions are
 *          different (case of colliding tasks).
 */
bool operator==(const CTask& task1, const CTask& task2) /*throw()*/
{
    /* Return true if the objects are the same */
    if (&task1 == &task2)
        return true;
    /* Do the actual comparison otherwise */
    if (task1.m_Time == task2.m_Time)
    {
        if (task1.m_Description != task2.m_Description)
            throw std::runtime_error("Colliding tasks!");
        return true;
    }
    return false;
}

bool operator!=(const CTask& task1, const CTask& task2) /*throw()*/
{
    return !(task1 == task2);
}


/* Overloaded injection operator - Used for display. */
std::ostream& operator<<(
    std::ostream& os,
    const CTask& task)
{
    tm* ptime = nullptr;

    /* General case, except for non-timed tasks */
    if (task.m_Time != -1)
        ptime = localtime(&task.m_Time);

    return (ptime ? (os << std::put_time(ptime, "%H:%M") << " -- ") : os)
            << (!task.m_Description.empty() ? task.m_Description : "n/a");
}


/**
 * @brief   Generic function for sorting std::queue<>'s in "non-descending
 *          (i.e. ascending)" order, so that the elements are popped out in
 *          increasing order.
 *
 *          NOTE: Implementation is not recursive.
 *          This function is written because we don't use the more
 *          sophisticated STL queue containers in this program...
 */
template <typename T>
void sort(std::queue<T>& queue)
{
    std::vector<T> tmpArray;
    while (!queue.empty())
    {
        tmpArray.push_back(queue.front());
        queue.pop();
    }
    /* NOTE: The comparison predicate must induce a strict weak ordering
     * relation; that's why we cannot use std::greater_equal<>, but need
     * to use std::greater<> instead. */
    std::sort(tmpArray.begin(), tmpArray.end(), std::greater<T>());
    while (!tmpArray.empty())
    {
        queue.push(tmpArray.back());
        tmpArray.pop_back();
    }
}


using std::cin;
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

    cout << "Usage: " << exeName <<
#ifdef TASK_RUN_SCHEDULE
            " [--run]"
#endif
            " <tasklistfile>\n\n"
         << "Parameters:\n"
#ifdef TASK_RUN_SCHEDULE
            "    -r, --run       Optional parameter. When set, schedule the list of tasks.\n"
            "                    Otherwise, enumerate the list of tasks without scheduling.\n"
#endif
            "    tasklistfile    Text file enumerating the list of tasks." << endl;
}
#endif

int main(int argc, char** argv)
{
#ifdef TASK_RUN_SCHEDULE
    bool bRun = false; // Default: don't run the tasks, just list them.
#endif

#ifdef TEST_MODE

    /* TEST MODE streams */
    std::istringstream strStream(testSchedule);
    std::ostream*   org_tiestr_cin = cin.tie(nullptr);
    std::streambuf* org_strbuf_cin = cin.rdbuf(strStream.rdbuf());

#else

    /*
     * Check for options.
     */
    int i;
    for (i = 1; i < argc; ++i)
    {
        if (
#ifdef _WIN32
// On Win32, support also the usual '/' switch.
            (*argv[i] != '/') &&
#endif
            (*argv[i] != '-'))
        {
            /* We are out of options (they come first before
             * anything else, and cannot come after). */
            break;
        }
        /* Short form ("-x") or long form ("--xxx") option? */
        bool bLongOpt =
#ifdef _WIN32
// Long option must always start with dash '-', so do this check on Win32.
            (argv[i][0] == '-') &&
#endif
            (argv[i][1] == '-');

        /* Help */
        if ((!bLongOpt && (strcmp(&argv[i][1], "?") == 0)) ||
            ( bLongOpt && (strcmp(&argv[i][2], "help") == 0)))
        {
            /* Set argc to special case value */
            argc = 0;
            break;
        }
#ifdef TASK_RUN_SCHEDULE
        else
        /* Run the tasks */
        if ((!bLongOpt && (strcmp(&argv[i][1], "r") == 0)) ||
            ( bLongOpt && (strcmp(&argv[i][2], "run") == 0)))
        {
            bRun = true;
        }
#endif
        else
        /* Unknown option */
        {
            cerr << "Unknown option: '" << argv[i] << "'\n" << endl;
            /* Set argc to special case value */
            argc = 0;
            break;
        }
    }
    /* Check for help / invalid syntax */
    if (argc == 0)
    {
        Usage(argv[0]);
        return -1;
    }


    /*
     * Check for a file or STDIN redirection.
     */
    std::ifstream inFile; // File stream, if any.
    std::ostream*   org_tiestr_cin = nullptr; // Original std::cin tied ostream.
    std::streambuf* org_strbuf_cin = nullptr; // Original std::cin stream buffer.

    if ((argc <= 1) || (i >= argc))
    {
        if (_isatty(_fileno(stdin)))
        {
            /* STDIN not redirected: we expected a file but didn't get one.
             * Display usage and bail out. */
            Usage(argv[0]);
            return ((argc <= 1) ? -1 : 0); // Use a different return code.
        }
        else
        {
            /* STDIN redirected: use it */
        }
    }
    else
    {
        /* Try to open the task list file for input */
        inFile.open(argv[i], std::ios::in);
        if (!inFile.is_open())
        {
            cerr << "Could not open task list file '" << argv[i] << "'" << endl;
            return -1;
        }

        /* Redirect std::cin to the file */
        org_tiestr_cin = cin.tie(nullptr);
        org_strbuf_cin = cin.rdbuf(inFile.rdbuf());
    }

#endif

    /* Set the current user's locale (for nicely displaying dates...) */
    cout.imbue(std::locale(""));

    /* Time support */
    time_t t_today = time(nullptr);
    tm tm_today = *localtime(&t_today);
    tm_today.tm_hour = tm_today.tm_min = tm_today.tm_sec = 0;
    t_today = mktime(&tm_today);

    /*
     * Parse the tasks from the input stream.
     */
    enum
    {
        SIMPLE_TASKS = 0,
        TIMED_TASKS,
        MAX_TYPE_TASKS
    };
    std::queue<CTask> tasks[MAX_TYPE_TASKS];
    while (true)
    {
        size_t pos;
        std::string line;
        std::string timestr, description;

        /* Try to read an entire line representing a task */
        std::getline(cin, line);

        /* Stop now if we reached the end of the stream */
        if (cin.eof())
            break;

        /*
         * Parse the line: "HH:MM <whitespace> Task_description"
         */
        constexpr const char* WHITESPACE = " \t\f\v\n\r";

        /* Trim leading whitespace */
        pos = line.find_first_not_of(WHITESPACE);
        line = line.substr(pos != std::string::npos ? pos : line.length());

        /* If we have a time string, it goes until the next whitespace */
        pos = line.find_first_of(WHITESPACE);
        timestr = line.substr(0, pos);

        /* Try to parse and recognize the time string.
         * NOTE: std::get_time() only modifies the necessary fields. */
        tm tm_time = tm_today;
#ifdef GCC_TIME_WORKAROUND
        if (strptime(timestr.c_str(), "%H:%M", &tm_time) == nullptr)
#else
        if ((std::istringstream(timestr) >> std::get_time(&tm_time, "%H:%M")).fail())
#endif
        {
            /* Parsing failed. The string is just the whole task description. */
            timestr.clear();
        }
        else
        {
            /* Parsing succeeded */

            /* Skip the time string */
            line = line.substr(pos != std::string::npos ? pos : line.length());
            /* Trim next whitespace */
            pos = line.find_first_not_of(WHITESPACE);
            line = line.substr(pos != std::string::npos ? pos : line.length());
        }

        /* Description string is everything else, without trailing whitespace */
        pos = line.find_last_not_of(WHITESPACE);
        description = line.substr(0, pos != std::string::npos ? pos + 1 : 0);

        /* If no description, skip this entry */
        if (description.empty())
            continue;

        /* Append this new task to the correct queue */
        // tasks.push(CTask(&tm_time, description));
        if (timestr.empty())
            tasks[SIMPLE_TASKS].emplace(description);
        else
            tasks[TIMED_TASKS].emplace(&tm_time, description);
    }

#ifndef TEST_MODE
    /* If we had a task file, close it */
    if (inFile.is_open())
        inFile.close();
#endif
    /* Restore any original std::cin stream */
    if (org_strbuf_cin)
        cin.rdbuf(org_strbuf_cin);
    if (org_tiestr_cin)
        cin.tie(org_tiestr_cin);


    /* Sort the timed task queue in "ascending" order */
    sort(tasks[TIMED_TASKS]);


    /* Print the header */
    cout << "==== Tasks for Today, "
         << std::put_time(localtime(&t_today), "%A %x")
#ifdef TASK_RUN_SCHEDULE
         << (bRun ? " [Run mode]" : "")
#endif
         << " ====\n" << endl;

    /* Save whether we originally had tasks to do (used for later) */
    bool bHadTasks = !tasks[SIMPLE_TASKS].empty() ||
                     !tasks[TIMED_TASKS].empty();

    /* First, enumerate any simple task we need to do */
    if (!tasks[SIMPLE_TASKS].empty())
    {
        cout << "To do:\n------\n" << endl;
        while (!tasks[SIMPLE_TASKS].empty())
        {
            /* Retrieve the next task and pop it */
            cout << tasks[SIMPLE_TASKS].front() << endl;
            tasks[SIMPLE_TASKS].pop();
        }
        cout << endl;
    }

    /* Then, run any scheduled timed task */
    if (!tasks[TIMED_TASKS].empty())
    {
        cout << "Scheduled tasks:\n----------------\n" << endl;
        while (!tasks[TIMED_TASKS].empty())
        {
            /* Do the next task and pop it */
#ifdef TASK_RUN_SCHEDULE
            if (bRun)
            {
                cout << "Currently doing:\n"
                        "  --> " << tasks[TIMED_TASKS].front() << endl;
            }
            else
#endif
            {
                cout << tasks[TIMED_TASKS].front() << endl;
            }
            tasks[TIMED_TASKS].pop();

#ifdef TASK_RUN_SCHEDULE
            if (bRun)
            {
                /* If the list is now empty, just quit */
                if (tasks[TIMED_TASKS].empty())
                    break;

                /* Otherwise, show what the next task will be... */
                CTask& task = tasks[TIMED_TASKS].front();
                cout << "The next task will be:\n"
                        "    [ " << task << " ]\n" << endl;

                /* and wait until the next task begins. */
                using std::chrono::system_clock;
                std::this_thread::sleep_until(system_clock::from_time_t(task.time()));
            }
#endif
        }
        cout << endl;
    }

    /* And we are done! */
    cout << (bHadTasks ? "You have finished all your tasks, congratulations! You've earned it!"
                       : "Nothing to do today! Relax & enjoy!") << endl;
    return 0;
}
