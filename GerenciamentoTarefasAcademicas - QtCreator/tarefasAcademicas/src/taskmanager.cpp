#include "taskmanager.h"

bool TaskManager::addTask(const Task &task)
{
    // impede o cadastro de tarefas incompletas
    if (!task.isValid()) {
        return false;
    }

    m_tasks.append(task);
    return true;
}

bool TaskManager::removeTask(int index)
{
    // protege a lista contra indices invalidos
    if (index < 0 || index >= m_tasks.size()) {
        return false;
    }

    m_tasks.removeAt(index);
    return true;
}

bool TaskManager::toggleCompleted(int index)
{
    // alterna o estado da tarefa selecionada
    if (index < 0 || index >= m_tasks.size()) {
        return false;
    }

    m_tasks[index].setCompleted(!m_tasks[index].isCompleted());
    return true;
}

void TaskManager::clear()
{
    m_tasks.clear();
}

QList<Task> TaskManager::tasks() const
{
    return m_tasks;
}

int TaskManager::totalCount() const
{
    return m_tasks.size();
}

int TaskManager::completedCount() const
{
    int count = 0;

    for (const Task &task : m_tasks) {
        if (task.isCompleted()) {
            ++count;
        }
    }

    return count;
}

int TaskManager::pendingCount() const
{
    return totalCount() - completedCount();
}
