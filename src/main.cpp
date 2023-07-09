#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#define HOURS_PER_DAY 24
#define TOTAL_AVAILABLE_HOURS 3 * HOURS_PER_DAY

class cTask
{
public:
    std::string name;
    int duration;
    int wStart;
    int wEnd;
    int actualStart;

    std::string text();
};

class cSchedule
{
public:
    std::vector<bool> vBusyHour;
    std::vector<cTask> vTask;

    cSchedule()
    {
        vBusyHour.resize(TOTAL_AVAILABLE_HOURS, false);
    }

    // add task to schedule
    void add(cTask &t);

private:
    // find free fragments in task window
    int freeFragments(const cTask &t);
    void split(cTask &t);
};

std::string cTask::text()
{
    std::stringstream ss;
    if (actualStart < 0)
    {
        ss << "task " << name << " cannot be scheduled\n";
    }
    else
    {
        int day = actualStart / HOURS_PER_DAY;
        int hour = actualStart - day * HOURS_PER_DAY;
        ss << "task " << name
           << " scheduled at " << hour << " on day " << day;
    }
    return ss.str();
}

void cSchedule::add(cTask &t)
{
    t.actualStart = -1;

    // loop over window
    for (
        int delay = 0;
        delay < t.wEnd - t.wStart - t.duration;
        delay++)
    {
        // check that no overlap with other task
        bool okBusy = true;
        int StartHour = t.wStart * HOURS_PER_DAY + delay;
        int EndHour = StartHour + t.duration;
        for (int hour = StartHour;
             hour < EndHour;
             hour++)
        {
            if (vBusyHour[hour])
            {
                // overlap
                okBusy = false;
                break;
            }
        }
        if (okBusy)
        {
            // add task to schedule
            for (int hour = StartHour; hour < EndHour; hour++)
                vBusyHour[hour] = true;
            t.actualStart = StartHour;

            break;
        }
    }


    if (t.actualStart < 0)
    {
        // no space big enough for task
        if (freeFragments(t) > t.duration)
        {
            std::cout << "splitting task\n";
            split( t );
        }
        else
            vTask.push_back(t);
    }
    else
        vTask.push_back(t);

    for (auto &t : vTask)
        std::cout << t.text() << "\n";
}

int cSchedule::freeFragments(const cTask &t)
{
    int total = 0;
    // loop over window
    for (
        int hour = t.wStart * HOURS_PER_DAY;
        hour <= t.wEnd * HOURS_PER_DAY;
        hour++)
    {
        if (!vBusyHour[hour])
            total++;
    }
    return total;
}

void cSchedule::split(cTask &t)
{
    int splitCount = 0;
    bool infragment = false;
    int fragmentStart;
    int fragmentEnd;

    // loop over hours in task window
    for (
        int hour = t.wStart * HOURS_PER_DAY;
        hour <= t.wEnd * HOURS_PER_DAY;
        hour++)
    {
        if (infragment)
        {
            if (!vBusyHour[hour])
            {
                // extend fragment
                fragmentEnd++;
            }
            else
            {
                // current fragment ended
                // split off part of task that fits into fragment
                splitCount++;
                cTask split;
                split.name = t.name + "-" + std::to_string(splitCount);
                split.actualStart = fragmentStart;
                vTask.push_back(split);

                // mark the busy hours
                int busyend = fragmentStart + t.duration;
                if( busyend > fragmentEnd )
                    busyend = fragmentEnd;
                for (
                    int makebusy = fragmentStart;
                    makebusy <= busyend;
                    makebusy++)
                    vBusyHour[makebusy] = true;

                // update remaining task duration
                t.duration -= busyend - fragmentStart;

                if( t.duration <= 0 ) {
                    // the task duration has completed
                    return;
                }
                infragment = false;
            }
        }
        else
        {
            if (!vBusyHour[hour])
            {
                // new fragment start
                fragmentStart = hour;
                fragmentEnd = hour;
                infragment = true;
            }
        }
    }
}
void readfile(
    cSchedule &S,
    const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error(
            "Cannot open input file");
    std::string type;
    cTask task;
    ifs >> type >> task.name >> task.duration >> task.wStart >> task.wEnd;
    while (ifs.good())
    {
        std::cout << "\nAdding "
                  << task.name << " " << task.duration << " " << task.wStart << " " << task.wEnd << "\n";

        S.add(task);

        ifs >> type >> task.name >> task.duration >> task.wStart >> task.wEnd;
    }
}

main(int argc, char *argv[])
{
    if (argc != 2)
        throw std::runtime_error(
            "Cannot open input file");

    cSchedule S;
    readfile(S, argv[1]);

    return 0;
}
