#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "task.h"

#include <QList>
#include <QString>

class TaskManager
{
public:
    bool addTask(const Task &task);
    bool removeTask(int index);
    bool toggleCompleted(int index);
    void clear();

    QList<Task> tasks() const;
    int totalCount() const;
    int completedCount() const;
    int pendingCount() const;

private:
    QList<Task> m_tasks;
};

#endif // taskmanager_h
