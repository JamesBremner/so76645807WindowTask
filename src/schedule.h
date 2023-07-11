
class cTask
{
public:
    std::string name;
    int duration;
    int actualStart;
    int minSplit;
    int fulfillment;

    void setWindow(int start, int end)
    {
        wStart = start;
        wEnd = end;
    }

    /// @brief set window start
    /// @param start 1-based day index

    void setStartWindowDay(int start)
    {
        wStart = (start - 1) * HOURS_PER_DAY;
    }

    /// @brief set window end
    /// @param end 1-based day index
    /// The end day is included in the window

    void setEndWindowDay(int end)
    {
        wEnd = end * HOURS_PER_DAY;
    }

    int getStartWindowTime() const
    {
        return wStart;
    }
    int getEndWindowTime() const
    {
        return wEnd;
    }
    int getLengthWindowTime() const
    {
        return wEnd - wStart;
    }
    std::string text() const;
    std::string textSchedule() const;

private:
    int wStart;
    int wEnd;
};

class cSchedule
{
public:
    cSchedule();

    // add task to schedule
    void add(cTask &t);
    void addW(cTask &t);

    void display() const;

    void unitTest();

private:
    std::vector<bool> vBusyHour;
    std::vector<cTask> vTask;

    /// @brief  find free time fragments in task window
    /// @param[in] t the task
    /// @param[out] vfrag free fragments
    /// @return total length of all free fragments

    int freeFragments(
        const cTask &t,
        std::vector<std::pair<int, int>> &vfrag);

    void sort(std::vector<std::pair<int, int>> &vfrag);

    // split task into free fragments of window
    void split(cTask &t);

    int totallowerfulfill(cTask &t);

    cTask &taskOccupy(int time);
};
