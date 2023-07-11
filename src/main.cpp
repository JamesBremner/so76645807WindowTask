#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#define HOURS_PER_DAY 24
#define TOTAL_AVAILABLE_HOURS 12

#include "schedule.h"

std::string cTask::textSchedule() const
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
           << " scheduled at hour " << hour << " on day " << day;
    }
    return ss.str();
}

std::string cTask::text() const
{
    std::stringstream ss;
    ss << " task:" << name
       << " duration " << duration
       << " windowstarthour:" << wStart
       << " windowendhour:" << wEnd << " ";
    //    << minSplit << " "
    //    << fulfillment;
    return ss.str();
}
void cSchedule::addW(cTask &t)
{
    std::cout << "\nAdding " << t.text() << "\n";

    t.actualStart = -1;

    // find smallest free time fragment in task window where task will fit
    std::vector<std::pair<int, int>> vfrag;
    freeFragments(t, vfrag);
    for (auto it = vfrag.begin();
         it != vfrag.end();
         it++)
    {
        auto db = (*it).first;
        int length = it->second - it->first + 1;
        if (length > t.duration)
        {
            // schedule task
            t.actualStart = it->first;
            for (int hour = it->first;
                 hour <= it->first + t.duration - 1;
                 hour++)
                vBusyHour[hour] = true;
            vTask.push_back(t);
            display();
            return;
        }
    }
    std::cout << "no space big enough for task\n";
    vTask.push_back(t);
}
void cSchedule::add(cTask &t)
{
    std::cout << "\nAdding " << t.text() << "\n";

    t.actualStart = -1;

    // loop over window
    for (
        int delay = 0;
        delay < t.getLengthWindowTime() - t.duration;
        delay++)
    {
        // check that no overlap with other task
        bool okBusy = true;
        int StartHour = t.getStartWindowTime() + delay;
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
            vTask.push_back(t);
            display();
            return;
        }
    }

    std::cout << "no space big enough for task\n";

    if (t.minSplit > 0)
    {
        std::vector<std::pair<int, int>> vfrag;
        if (freeFragments(t, vfrag) > t.duration)
        {
            std::cout << "splitting task\n";
            split(t);
            display();
            return;
        }
    }

    if (t.actualStart < 0)
    {
        if (totallowerfulfill(t) > t.duration)
        {
            std::cout << "I can bump tasks with lower fulfillment scores\n";
        }
        vTask.push_back(t);
    }
}

void cSchedule::display() const
{
    for (auto &t : vTask)
        std::cout << t.textSchedule() << "\n";

    for (bool b : vBusyHour)
    {
        if (b)
            std::cout << 'B';
        else
            std::cout << '_';
    }
    std::cout << "\n";
}

cTask &cSchedule::taskOccupy(int time)
{
    // loop over tasks
    for (auto &t : vTask)
    {
        if (t.actualStart < 0)
            continue;
        if (t.actualStart > time)
            continue;
        if (t.actualStart + t.duration > time)
            return t;
    }
    static cTask nullTask;
    nullTask.actualStart = -1;
    return nullTask;
}

int cSchedule::totallowerfulfill(cTask &t)
{
    int total = 0;

    // loop over window
    for (
        int hour = t.getStartWindowTime();
        hour <= t.getEndWindowTime();
        hour++)
    {
        cTask &o = taskOccupy(hour);
        if (o.actualStart < 0)
            continue;
        if (o.fulfillment >= t.fulfillment)
            continue;
        total++;
    }
    return total;
}

int cSchedule::freeFragments(
    const cTask &t,
    std::vector<std::pair<int, int>> &vfrag)
{
    int total = 0; // total length of all free fragmets

    bool infragment = false;
    int fragmentStart;
    int fragmentEnd;

    // loop over window
    for (
        int hour = t.getStartWindowTime();
        hour <= t.getEndWindowTime();
        hour++)
    {
        if (infragment)
        {
            if (!vBusyHour[hour] &&
                hour != t.getEndWindowTime())

                fragmentEnd++; // extend fragment

            else
            {
                // free fragment has ended
                vfrag.push_back(
                    std::make_pair(
                        fragmentStart, fragmentEnd));

                // check that fragment is not too small for minimum split
                int length = fragmentEnd - fragmentStart + 1;
                if (length >= t.minSplit)
                    total += length;

                infragment = false;
            }
        }
        else
        {
            if (!vBusyHour[hour])
            {
                // new fragment started
                fragmentStart = hour;
                fragmentEnd = hour;
                infragment = true;
            }
        }
    }

    /// sort free fragments into descending length order
    sort(vfrag);

    return total;
}

void cSchedule::sort(std::vector<std::pair<int, int>> &vfrag)
{
    std::sort(vfrag.begin(), vfrag.end(),
              [](std::pair<int, int> &l, std::pair<int, int> &r) -> bool
              {
                  int ll = l.second - l.first + 1;
                  int lr = r.second - r.first + 1;
                  return ll < lr;
              });
}

void cSchedule::split(cTask &t)
{
    int splitCount = 0;
    bool infragment = false;
    int fragmentStart;
    int fragmentEnd;

    // loop over hours in task window
    for (
        int hour = t.getStartWindowTime();
        hour <= t.getEndWindowTime();
        hour++)
    {
        if (infragment)
        {
            if (!vBusyHour[hour] &&
                hour != t.getEndWindowTime())
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
                if (busyend > fragmentEnd)
                    busyend = fragmentEnd;
                for (
                    int makebusy = fragmentStart;
                    makebusy <= busyend;
                    makebusy++)
                    vBusyHour[makebusy] = true;

                // update remaining task duration
                t.duration -= busyend - fragmentStart;

                if (t.duration <= 0)
                {
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

void cSchedule::unitTest()
{
    std::vector<std::pair<int, int>> vfrag{
        {4, 10},
        {1, 2}
    sort(vfrag);
    if (vfrag[0].first != 1)
        throw std::runtime_error(
            "sort failed unit test");
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

    int wStart, wEnd;

    while (1)
    {
        ifs >> type >> task.name >> task.duration >> wStart >> wEnd >> task.minSplit >> task.fulfillment;
        if (!ifs.good())
            break;

        task.setWindow(wStart, wEnd);

        S.addW(task);
    }
}

cSchedule::cSchedule()
{
    vBusyHour.resize(TOTAL_AVAILABLE_HOURS, false);

    unitTest();
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
