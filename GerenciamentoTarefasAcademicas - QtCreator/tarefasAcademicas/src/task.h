#ifndef TASK_H
#define TASK_H

#include <QDate>
#include <QJsonObject>
#include <QString>

class Task
{
public:
    enum class Priority
    {
        Low = 0,
        Medium = 1,
        High = 2
    };

    Task();
    Task(const QString &title, const QString &description, const QDate &dueDate, Priority priority, bool completed = false);

    QString title() const;
    QString description() const;
    QDate dueDate() const;
    Priority priority() const;
    bool isCompleted() const;

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setDueDate(const QDate &dueDate);
    void setPriority(Priority priority);
    void setCompleted(bool completed);

    bool isValid() const;
    QString statusText() const;

    QJsonObject toJson() const;
    static Task fromJson(const QJsonObject &object);

    static QString priorityToText(Priority priority);
    static Priority priorityFromIndex(int index);

private:
    QString m_title;
    QString m_description;
    QDate m_dueDate;
    Priority m_priority;
    bool m_completed;
};

#endif // task_h
