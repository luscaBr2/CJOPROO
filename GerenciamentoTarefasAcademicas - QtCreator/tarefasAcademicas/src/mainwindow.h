#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "taskfilerepository.h"
#include "taskmanager.h"

#include <QComboBox>
#include <QDateEdit>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    void buildInterface();
    void createConnections();
    void updateTable();
    void updateSummary();
    void clearForm();
    int selectedRow() const;

    void addTask();
    void removeSelectedTask();
    void toggleSelectedTask();
    void saveTasks();
    void loadTasks();

    TaskManager m_taskManager;
    TaskFileRepository m_repository;

    QLineEdit *m_titleEdit;
    QTextEdit *m_descriptionEdit;
    QDateEdit *m_dueDateEdit;
    QComboBox *m_priorityCombo;
    QTableWidget *m_table;
    QLabel *m_summaryLabel;
    QLabel *m_storageLabel;
    QPushButton *m_addButton;
    QPushButton *m_removeButton;
    QPushButton *m_toggleButton;
    QPushButton *m_clearButton;
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;
};

#endif // mainwindow_h
