#include "taskfilerepository.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QSaveFile>
#include <QStandardPaths>

TaskFileRepository::TaskFileRepository(const QString &filePath)
    : m_filePath(filePath.isEmpty() ? defaultFilePath() : filePath)
{
}

bool TaskFileRepository::save(const QList<Task> &tasks, QString *errorMessage) const
{
    // cria a pasta de dados antes de salvar o arquivo
    const QFileInfo fileInfo(m_filePath);
    QDir().mkpath(fileInfo.absolutePath());

    QJsonArray array;

    for (const Task &task : tasks) {
        array.append(task.toJson());
    }

    QSaveFile file(m_filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMessage != nullptr) {
            *errorMessage = "Nao foi possivel abrir o arquivo para escrita.";
        }
        return false;
    }

    file.write(QJsonDocument(array).toJson(QJsonDocument::Indented));

    if (!file.commit()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Nao foi possivel concluir a gravacao do arquivo.";
        }
        return false;
    }

    return true;
}

bool TaskFileRepository::load(QList<Task> *tasks, QString *errorMessage) const
{
    // carrega os dados somente quando o ponteiro de destino e valido
    if (tasks == nullptr) {
        if (errorMessage != nullptr) {
            *errorMessage = "Lista de destino invalida.";
        }
        return false;
    }

    QFile file(m_filePath);

    if (!file.exists()) {
        tasks->clear();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage != nullptr) {
            *errorMessage = "Nao foi possivel abrir o arquivo para leitura.";
        }
        return false;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());

    if (!document.isArray()) {
        if (errorMessage != nullptr) {
            *errorMessage = "O arquivo possui um formato invalido.";
        }
        return false;
    }

    QList<Task> loadedTasks;

    for (const QJsonValue &value : document.array()) {
        const Task task = Task::fromJson(value.toObject());

        if (task.isValid()) {
            loadedTasks.append(task);
        }
    }

    *tasks = loadedTasks;
    return true;
}

QString TaskFileRepository::filePath() const
{
    return m_filePath;
}

QString TaskFileRepository::defaultFilePath() const
{
    // usa uma pasta padrao do sistema operacional para os dados do app
    const QString dataLocation = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(dataLocation).filePath("tarefas.json");
}
