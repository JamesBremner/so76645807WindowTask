# Schedule tasks with windows

- A task has a duration
- A task has a window, period during which the task must be started and completed.
- A task can be split into subtasks, all must be started and completed inside task window
- A task has a minimum duration of subtasks.  0 means task cannot be split.
- Tasks can be scheduled in any order
- Tasks are added one at a time.  New tasks cannot alter the scheduling of previously added tasks.
- There is a single task processor that handles one task at a time.
