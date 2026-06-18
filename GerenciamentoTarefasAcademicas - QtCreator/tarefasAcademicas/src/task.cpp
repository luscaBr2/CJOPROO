#include "task.h"

Task::Task()
    : m_dueDate(QDate::currentDate()),
      m_priority(Priority::Medium),
      m_completed(false)
{
}

Task::Task(const QString &title, const QString &description, const QDate &dueDate, Priority priority, bool completed)
    : m_title(title.trimmed()),
      m_description(description.trimmed()),
      m_dueDate(dueDate),
      m_priority(priority),
      m_completed(completed)
{
}

QString Task::title() const
{
    return m_title;
}

QString Task::description() const
{
    return m_description;
}

QDate Task::dueDate() const
{
    return m_dueDate;
}

Task::Priority Task::priority() const
{
    return m_priority;
}

bool Task::isCompleted() const
{
    return m_completed;
}

void Task::setTitle(const QString &title)
{
    m_title = title.trimmed();
}

void Task::setDescription(const QString &description)
{
    m_description = description.trimmed();
}

void Task::setDueDate(const QDate &dueDate)
{
    m_dueDate = dueDate;
}

void Task::setPriority(Priority priority)
{
    m_priority = priority;
}

void Task::setCompleted(bool completed)
{
    m_completed = completed;
}

bool Task::isValid() const
{
    // verifica se os dados essenciais foram informados
    return !m_title.trimmed().isEmpty() && m_dueDate.isValid();
}

QString Task::statusText() const
{
    return m_completed ? "Concluida" : "Pendente";
}

QJsonObject Task::toJson() const
{
    // converte o objeto para um formato simples de persistencia
    QJsonObject object;
    object["title"] = m_title;
    object["description"] = m_description;
    object["dueDate"] = m_dueDate.toString(Qt::ISODate);
    object["priority"] = static_cast<int>(m_priority);
    object["completed"] = m_completed;
    return object;
}

Task Task::fromJson(const QJsonObject &object)
{
    // recria uma tarefa a partir dos dados salvos em arquivo
    const QString title = object.value("title").toString();
    const QString description = object.value("description").toString();
    const QDate dueDate = QDate::fromString(object.value("dueDate").toString(), Qt::ISODate);
    const auto priority = priorityFromIndex(object.value("priority").toInt(1));
    const bool completed = object.value("completed").toBool(false);
    return Task(title, description, dueDate, priority, completed);
}

QString Task::priorityToText(Priority priority)
{
    switch (priority) {
    case Priority::Low:
        return "Baixa";
    case Priority::Medium:
        return "Media";
    case Priority::High:
        return "Alta";
    }

    return "Media";
}

Task::Priority Task::priorityFromIndex(int index)
{
    switch (index) {
    case 0:
        return Priority::Low;
    case 2:
        return Priority::High;
    default:
        return Priority::Medium;
    }
}
