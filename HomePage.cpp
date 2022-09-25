#include "HomePage.h"
#include "ui_HomePage.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

#include "utils/Theme.h"

using namespace HomePage;

// CONSTRUCTOR ///////////////////////////////////////////////////////////

Plugin::Plugin(QMainWindow* parent) :
    QWidget(parent),
    ui(new Ui::HomePage)
  , parent(parent)
{
    ui->setupUi(this);
    ui->lblTitle->setText(tr("Welcome to") + " <font color='" + Theme::getColor("@active") + "'>" + qApp->applicationName() + "</font>");
    ui->btNewFile->setEnabled(false);
    ui->btOpenFile->setEnabled(false);

    ui->leftMenu->layout()->removeWidget(ui->btExit);

    ui_switchSaveProject = new QSwitchButton("Open last project");
    ui_switchSaveProject->style.font = QFont("Ubuntu", 10);
    ui_switchSaveProject->init();
    connect(ui_switchSaveProject, &QSwitchButton::toggled, this, [=](bool checked)
    {
        QSettings settings;
        if (checked) {
            settings.setValue("homepage/saveproject", true);
            this->m_openLastProject = true;
        } else {
            settings.remove("homepage/saveproject");
            this->m_openLastProject = false;
        }
    });
    ui->leftMenu->layout()->addWidget(ui_switchSaveProject);

    ui_switchSaveFile = new QSwitchButton("Open last file");
    ui_switchSaveFile->style.font = QFont("Ubuntu", 10);
    ui_switchSaveFile->init();
    connect(ui_switchSaveFile, &QSwitchButton::toggled, this, [=](bool checked)
    {
        QSettings settings;
        if (checked) {
            settings.setValue("homepage/savefile", true);
            this->m_openLastFile = true;
        } else {
            settings.remove("homepage/savefile");
            this->m_openLastFile = false;
        }
    });
    ui->leftMenu->layout()->addWidget(ui_switchSaveFile);

    ui->leftMenu->layout()->addWidget(ui->btExit);

    this->initUi();

    connect(ui->btNewProject, &QPushButton::clicked, this, &Plugin::onNewProject);
    connect(ui->btNewFile, &QPushButton::clicked, this, &Plugin::onNewFile);
    connect(ui->btOpenFile, &QPushButton::clicked, this, &Plugin::onOpenFile);
    connect(ui->btExit, &QPushButton::clicked, this, [=](){
        parent->close();
    });

    this->m_openLastProject = true;
    this->m_openLastFile = false;
    this->m_fileTypeExtension = "json";
    this->m_fileTypeName = tr("JSON files");
    this->m_fileDefault = "{}";
    this->current_project = nullptr;
    this->current_file = nullptr;
    this->load();
}

Plugin::~Plugin()
{
    delete ui;
}

void Plugin::load()
{
    QSettings settings;

    settings.beginGroup("homepage/project");
    auto keys = settings.allKeys();
    keys.sort();
    for (const QString& code: keys) {
        this->addProject(true, settings.value(code).toJsonObject());
    }
    settings.endGroup();

    this->m_files.clear();
    settings.beginGroup("homepage/file");
    // TODO SORT BY TITLE
    for (const QString& code: settings.allKeys()) {
        this->addFile(settings.value(code).toJsonObject());
    }
    settings.endGroup();

    this->m_openLastProject = settings.contains("homepage/saveproject");
    ui_switchSaveProject->setChecked(this->m_openLastProject);

    this->m_openLastFile = settings.contains("homepage/savefile");
    ui_switchSaveFile->setChecked(this->m_openLastFile);

    if (this->m_openLastProject && settings.contains("homepage/currentproject"))
    {
        const QString& code = settings.value("homepage/currentproject").toString();
        auto projects = ui->projects->findChildren<ProjectWidget*>();
        for (auto p: projects) {
            if (p->code() == code) {
                p->select();
                this->current_project = p->project();
            }
        }
    }

    if (this->m_openLastFile && settings.contains("homepage/currentfile")) {
        const QString& id = settings.value("homepage/currentfile").toString();
        for (auto file: this->m_files) {
            if (file->id() == id) {
                this->current_file = file;
            }
        }
    }
}

void Plugin::reload()
{
    auto projects = ui->projects->findChildren<ProjectWidget*>();
    for (auto project: projects) {
        project->close();
        project->deleteLater();
    }
    auto files = ui->files->findChildren<FileWidget*>();
    for (auto file: files) {
        file->close();
        file->deleteLater();
    }
    this->load();

    this->current_project = nullptr;
    QSettings settings;
    settings.remove("homepage/currentproject");
    ui->btNewFile->setEnabled(false);
    ui->btOpenFile->setEnabled(false);
}

void Plugin::initUi()
{
    main = parent->centralWidget();

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(this);
    layout->addWidget(parent->centralWidget());

    QWidget* global = new QWidget();
    global->setLayout(layout);

    home = this;
    home->setVisible(false);

    parent->setCentralWidget(global);
}

void Plugin::show()
{
    home->setVisible(true);
    main->setVisible(false);

    if (this->m_openLastFile && this->current_file != nullptr) {
        this->onFileSelected(this->current_file);
    }
}

void Plugin::hide()
{
    home->setVisible(false);
    main->setVisible(true);
}

void Plugin::closeCurrentFile()
{
    this->current_file = nullptr;
    if (this->m_openLastFile) {
        QSettings settings;
        settings.remove("homepage/currentfile");
    }

    this->show();
}


// PROJECTS //////////////////////////////////////////////////////////////

void Plugin::addProject(bool hasProject, const QJsonObject& project)
{
    ProjectWidget* pi = new ProjectWidget(ui->projects);
    if (hasProject) pi->setProject(project);
    connect(pi, &ProjectWidget::created, this, &Plugin::onProjectCreated);
    connect(pi, &ProjectWidget::selected, this, &Plugin::onProjectSelected);
    connect(pi, &ProjectWidget::deleted, this, &Plugin::onProjectDeleted);
    connect(pi, &ProjectWidget::updated, this, &Plugin::onProjectUpdated);
    auto layout = static_cast<QVBoxLayout*>(ui->projects->layout());
    layout->addWidget(pi);
}


void Plugin::onNewProject()
{
    this->addProject();
}

void Plugin::onProjectCreated(Project* project)
{
    this->reload();

    const QString& code = project->code();
    auto projects = ui->projects->findChildren<ProjectWidget*>();
    for (auto p: projects) {
        if (p->code() != code) {
            p->unselect();
        } else {
            p->select();
            this->current_project = p->project();
        }
    }
}

void Plugin::onProjectSelected(Project* project)
{
    if (project == nullptr) return;

    auto projects = ui->projects->findChildren<ProjectWidget*>();
    for (auto p: projects) {
        if (p->code() != project->code()) {
            p->unselect();
        } else {
            this->current_project = project;

            if (this->m_openLastProject) {
                QSettings settings;
                settings.setValue("homepage/currentproject", this->current_project->code());
            }
        }
    }

    ui->btNewFile->setEnabled(true);
    ui->btOpenFile->setEnabled(true);

    auto files = ui->files->findChildren<FileWidget*>();
    for (auto file: files) {
        file->close();
        file->deleteLater();
    }

    if (this->current_project != nullptr) {
        for (auto file: this->m_files) {
            if (file->project() == this->current_project) {
                this->addFileToUi(file);
            }
        }
    }
}

void Plugin::onProjectDeleted(Project* /*project*/)
{
    this->reload();
}

void Plugin::onProjectUpdated(Project* project)
{
    this->reload();

    const QString& code = project->code();
    auto projects = ui->projects->findChildren<ProjectWidget*>();
    for (auto p: projects) {
        if (p->code() != code) {
            p->unselect();
        } else {
            p->select();
            this->current_project = p->project();
        }
    }
}


// FILES /////////////////////////////////////////////////////////////////

void Plugin::addFile(const QJsonObject& file)
{
    const QString& id = file.value("id").toString();
    const QString& code = file.value("projectcode").toString();
    const QString& title = file.value("title").toString();
    const QString& path = file.value("path").toString();

    auto projects = ui->projects->findChildren<ProjectWidget*>();
    for (auto p: projects) {
        if (p->code() == code) {
            this->m_files.append(new File(id, p->project(), title, path));
        }
    }
    this->onProjectSelected(this->current_project);
}

void Plugin::createFile(const QString& title, const QString& path)
{
    const QString& id = QUuid::createUuid().toString(QUuid::WithoutBraces);

    File* file = new File(id, this->current_project, title, path);

    QSettings settings;
    settings.setValue("homepage/file/" + id, file->toJson());

    this->m_files.append(file);
    this->onProjectSelected(this->current_project);
}

void Plugin::addFileToUi(File* file)
{
    FileWidget* fw = new FileWidget(ui->files);
    fw->setFile(file);
    connect(fw, &FileWidget::deleted, this, &Plugin::onFileDeleted);
    connect(fw, &FileWidget::updated, this, &Plugin::onFileUpdated);
    connect(fw, &FileWidget::doubleclicked, this, &Plugin::onFileSelected);
    auto layout = static_cast<QVBoxLayout*>(ui->files->layout());
    layout->addWidget(fw);
}

void Plugin::onNewFile()
{
    QString fileType = this->m_fileTypeName + " (*." + this->m_fileTypeExtension + ")";

    auto fileName = QFileDialog::getSaveFileName(this, qApp->applicationName(), QDir::homePath(), fileType);
    if (fileName != "")
    {
        if (!fileName.endsWith("." + this->m_fileTypeExtension)) fileName += "." + this->m_fileTypeExtension;

        QFile f(fileName);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, qApp->applicationName(), tr("Error when saving file"));
            return;
        }
        QTextStream out(&f);
        out << this->m_fileDefault;
        f.close();

        QString fileTitle = QFileInfo(fileName).baseName();
        this->createFile(fileTitle, fileName);
    }
}

void Plugin::onOpenFile()
{
    QString fileType = this->m_fileTypeName + " (*." + this->m_fileTypeExtension + ")";

    auto fileName = QFileDialog::getOpenFileName(this, qApp->applicationName(), QDir::homePath(), fileType);
    if (fileName != "")
    {
        const QString& id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        const QString& title = QFileInfo(fileName).baseName();

        File* file = new File(id, this->current_project, title, fileName);

        QSettings settings;
        settings.setValue("homepage/file/" + id, file->toJson());

        this->m_files.append(file);
        this->onProjectSelected(this->current_project);
    }
}

void Plugin::onFileDeleted(File* file)
{
    // TODO Option to delete file on disk
    this->m_files.removeOne(file);
    QSettings settings;
    settings.remove("homepage/file/" + file->id());
}

void Plugin::onFileUpdated(File* file)
{
    QSettings settings;
    settings.setValue("homepage/file/" + file->id(), file->toJson());
}

void Plugin::onFileSelected(File* file)
{
    this->current_file = file;
    if (this->m_openLastFile) {
        QSettings settings;
        settings.setValue("homepage/currentfile", file->id());
    }

    this->hide();
    emit this->fileOpened(file);
}



