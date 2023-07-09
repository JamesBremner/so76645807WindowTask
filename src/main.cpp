#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#define HOURS_PER_DAY 24
#define TOTAL_AVAILABLE_HOURS 30 * HOURS_PER_DAY

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
    void add(cTask &t);
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

    vTask.push_back( t );

    for (auto &t : vTask)
        std::cout << t.text() << "\n";
}


void readfile(
    cSchedule& S,
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
    readfile(S,argv[1]);

    return 0;
}
