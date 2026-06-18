#ifndef TASKFILEREPOSITORY_H
#define TASKFILEREPOSITORY_H

#include "task.h"

#include <QList>
#include <QString>

class TaskFileRepository
{
public:
    explicit TaskFileRepository(const QString &filePath = QString());

    bool save(const QList<Task> &tasks, QString *errorMessage = nullptr) const;
    bool load(QList<Task> *tasks, QString *errorMessage = nullptr) const;
    QString filePath() const;

private:
    QString defaultFilePath() const;

    QString m_filePath;
};

#endif // taskfilerepository_h
