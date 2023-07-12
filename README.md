# Schedule tasks with windows

- A task has a duration
- A task has a window, period during which the task must be started and completed.
- A task can be split into subtasks, all must be started and completed inside task window
- A task has a minimum duration of subtasks.  0 means task cannot be split.
- A task has a fulfillment score.  If a task cannot be scheduled, a check is done for enough scheduled tasks with lower scores that the new task could be scheduled if the low score tasks were bumped.
- Tasks can be scheduled in any order
- Tasks are added one at a time.  New tasks do not alter the scheduling of previously added tasks, unless there is no alternative.
- There is a single task processor that handles one task at a time.
- The display shows the busy hours ( B ) and the free hours ( _ ).
- The display shows the fulfillments score and context switch count for each day.
