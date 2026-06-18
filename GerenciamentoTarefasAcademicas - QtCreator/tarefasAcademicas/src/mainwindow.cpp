#include "mainwindow.h"

#include <QAbstractItemView>
#include <QDate>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_titleEdit(nullptr),
      m_descriptionEdit(nullptr),
      m_dueDateEdit(nullptr),
      m_priorityCombo(nullptr),
      m_table(nullptr),
      m_summaryLabel(nullptr),
      m_storageLabel(nullptr),
      m_addButton(nullptr),
      m_removeButton(nullptr),
      m_toggleButton(nullptr),
      m_clearButton(nullptr),
      m_saveButton(nullptr),
      m_loadButton(nullptr)
{
    buildInterface();
    createConnections();
    loadTasks();
}

void MainWindow::buildInterface()
{
    // cria a janela principal usando layouts do qt widgets
    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    auto *formGroup = new QGroupBox("Cadastro de tarefa", central);
    auto *formLayout = new QFormLayout(formGroup);

    m_titleEdit = new QLineEdit(formGroup);
    m_titleEdit->setPlaceholderText("Ex.: estudar heranca em c++");

    m_descriptionEdit = new QTextEdit(formGroup);
    m_descriptionEdit->setPlaceholderText("Descreva a tarefa de forma curta");
    m_descriptionEdit->setFixedHeight(70);

    m_dueDateEdit = new QDateEdit(QDate::currentDate(), formGroup);
    m_dueDateEdit->setCalendarPopup(true);
    m_dueDateEdit->setDisplayFormat("dd/MM/yyyy");

    m_priorityCombo = new QComboBox(formGroup);
    m_priorityCombo->addItems({"Baixa", "Media", "Alta"});
    m_priorityCombo->setCurrentIndex(1);

    formLayout->addRow("Titulo:", m_titleEdit);
    formLayout->addRow("Descricao:", m_descriptionEdit);
    formLayout->addRow("Prazo:", m_dueDateEdit);
    formLayout->addRow("Prioridade:", m_priorityCombo);

    auto *buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Adicionar", central);
    m_removeButton = new QPushButton("Remover selecionada", central);
    m_toggleButton = new QPushButton("Alternar status", central);
    m_clearButton = new QPushButton("Limpar campos", central);
    m_saveButton = new QPushButton("Salvar", central);
    m_loadButton = new QPushButton("Carregar", central);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    buttonLayout->addWidget(m_toggleButton);
    buttonLayout->addWidget(m_clearButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_saveButton);
    buttonLayout->addWidget(m_loadButton);

    m_table = new QTableWidget(0, 5, central);
    m_table->setHorizontalHeaderLabels({"Titulo", "Descricao", "Prazo", "Prioridade", "Status"});
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_summaryLabel = new QLabel(central);
    m_storageLabel = new QLabel("Arquivo: " + m_repository.filePath(), central);
    m_storageLabel->setWordWrap(true);

    mainLayout->addWidget(formGroup);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_table);
    mainLayout->addWidget(m_summaryLabel);
    mainLayout->addWidget(m_storageLabel);

    setCentralWidget(central);
    setWindowTitle("Sistema de Tarefas Academicas");
    resize(980, 620);
    updateSummary();
}

void MainWindow::createConnections()
{
    // conecta botoes e acoes da interface aos metodos da janela
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::removeSelectedTask);
    connect(m_toggleButton, &QPushButton::clicked, this, &MainWindow::toggleSelectedTask);
    connect(m_clearButton, &QPushButton::clicked, this, &MainWindow::clearForm);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::saveTasks);
    connect(m_loadButton, &QPushButton::clicked, this, &MainWindow::loadTasks);
}

void MainWindow::updateTable()
{
    // reconstrui a tabela com base na lista atual de tarefas
    const QList<Task> tasks = m_taskManager.tasks();
    m_table->setRowCount(tasks.size());

    for (int row = 0; row < tasks.size(); ++row) {
        const Task &task = tasks.at(row);
        m_table->setItem(row, 0, new QTableWidgetItem(task.title()));
        m_table->setItem(row, 1, new QTableWidgetItem(task.description()));
        m_table->setItem(row, 2, new QTableWidgetItem(task.dueDate().toString("dd/MM/yyyy")));
        m_table->setItem(row, 3, new QTableWidgetItem(Task::priorityToText(task.priority())));
        m_table->setItem(row, 4, new QTableWidgetItem(task.statusText()));
    }

    updateSummary();
}

void MainWindow::updateSummary()
{
    // apresenta um resumo quantitativo para o usuario
    m_summaryLabel->setText(QString("Total: %1 | Pendentes: %2 | Concluidas: %3")
                                .arg(m_taskManager.totalCount())
                                .arg(m_taskManager.pendingCount())
                                .arg(m_taskManager.completedCount()));
}

void MainWindow::clearForm()
{
    // devolve o formulario ao estado inicial
    m_titleEdit->clear();
    m_descriptionEdit->clear();
    m_dueDateEdit->setDate(QDate::currentDate());
    m_priorityCombo->setCurrentIndex(1);
    m_titleEdit->setFocus();
}

int MainWindow::selectedRow() const
{
    const QList<QTableWidgetSelectionRange> ranges = m_table->selectedRanges();

    if (ranges.isEmpty()) {
        return -1;
    }

    return ranges.first().topRow();
}

void MainWindow::addTask()
{
    // transforma os dados do formulario em um objeto task
    const Task task(m_titleEdit->text(),
                    m_descriptionEdit->toPlainText(),
                    m_dueDateEdit->date(),
                    Task::priorityFromIndex(m_priorityCombo->currentIndex()));

    if (!m_taskManager.addTask(task)) {
        QMessageBox::warning(this, "Validacao", "Informe ao menos o titulo da tarefa.");
        return;
    }

    updateTable();
    clearForm();
    statusBar()->showMessage("Tarefa adicionada.", 3000);
}

void MainWindow::removeSelectedTask()
{
    // remove apenas a linha selecionada pelo usuario
    if (!m_taskManager.removeTask(selectedRow())) {
        QMessageBox::information(this, "Remocao", "Selecione uma tarefa para remover.");
        return;
    }

    updateTable();
    statusBar()->showMessage("Tarefa removida.", 3000);
}

void MainWindow::toggleSelectedTask()
{
    // marca uma tarefa como concluida ou pendente
    if (!m_taskManager.toggleCompleted(selectedRow())) {
        QMessageBox::information(this, "Status", "Selecione uma tarefa para alterar o status.");
        return;
    }

    updateTable();
    statusBar()->showMessage("Status atualizado.", 3000);
}

void MainWindow::saveTasks()
{
    // salva os dados em json usando o repositorio do sistema
    QString errorMessage;

    if (!m_repository.save(m_taskManager.tasks(), &errorMessage)) {
        QMessageBox::critical(this, "Erro ao salvar", errorMessage);
        return;
    }

    statusBar()->showMessage("Tarefas salvas com sucesso.", 3000);
}

void MainWindow::loadTasks()
{
    // carrega o arquivo json e atualiza a tabela visual
    QList<Task> tasks;
    QString errorMessage;

    if (!m_repository.load(&tasks, &errorMessage)) {
        QMessageBox::critical(this, "Erro ao carregar", errorMessage);
        return;
    }

    m_taskManager.clear();

    for (const Task &task : tasks) {
        m_taskManager.addTask(task);
    }

    updateTable();
    statusBar()->showMessage("Dados carregados.", 3000);
}
