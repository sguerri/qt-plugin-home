#include "PHomePage.h"
#include "ui_PHomePage.h"

#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>

#include "utils/font.h"

using namespace pluginHome::utils;





PCentralWidget::PCentralWidget(QMainWindow* parent, PHomePage* page) :
    QWidget(parent)
  , m_page(page)
{}

PCentralWidget::~PCentralWidget() {}

void PCentralWidget::showEvent(QShowEvent* event)
{
    //qDebug() << "SHOW" << this->m_page->hasCurrentFile();
    if (!this->m_page->hasCurrentFile()) {
        event->ignore();
        this->m_page->show();
    } else {
        event->accept();
    }
}










// CONSTRUCTOR ///////////////////////////////////////////////////////////

PHomePage::PHomePage(QMainWindow* parent, std::function<void (PHomePageFile*)> fn) :
    QWidget(parent),
    ui(new Ui::PHomePage)
  , parent(parent)
{
    parent->setProperty("hasHomePage", true);

    connect(this, &PHomePage::fileOpened, parent, fn);

    // TODO Use this to display settings page button
    // qDebug() << parent->property("hasSettingsPage");

    ui->setupUi(this);
    ui->lblTitle->setText(qApp->applicationName());
    ui->btNewFile->setEnabled(false);
    ui->btOpenFile->setEnabled(false);

    ui->leftMenu->layout()->removeWidget(ui->btExit);
    ui->leftMenu->layout()->removeWidget(ui->btSettings);

    ui_switchSaveProject = new CSwitchButton(tr("Open last project"));
    ui_switchSaveProject->style.font = smallerFont(ui->btNewProject->font(), 2);
    ui_switchSaveProject->init();
    connect(ui_switchSaveProject, &CSwitchButton::toggled, this, [=](bool checked)
    {
        QSettings settings;
        if (checked) {
            settings.setValue("application/homepage/saveproject", true);
            this->m_openLastProject = true;
        } else {
            settings.remove("application/homepage/saveproject");
            this->m_openLastProject = false;
        }
    });
    ui->leftMenu->layout()->addWidget(ui_switchSaveProject);

    ui_switchSaveFile = new CSwitchButton(tr("Open last file"));
    ui_switchSaveFile->style.font = smallerFont(ui->btNewProject->font(), 2);
    ui_switchSaveFile->init();
    connect(ui_switchSaveFile, &CSwitchButton::toggled, this, [=](bool checked)
    {
        QSettings settings;
        if (checked) {
            settings.setValue("application/homepage/savefile", true);
            this->m_openLastFile = true;
        } else {
            settings.remove("application/homepage/savefile");
            this->m_openLastFile = false;
        }
    });
    ui->leftMenu->layout()->addWidget(ui_switchSaveFile);

    if (parent->property("hasSettingsPage").toBool()) {
        ui->leftMenu->layout()->addWidget(ui->btSettings);
        connect(ui->btSettings, &QPushButton::clicked, this, [=](){
            for (auto c: parent->centralWidget()->children()) {
                if (c->objectName() == "centralwidget") {
                    static_cast<QWidget*>(c)->setVisible(false);
                } else if (c->objectName() == "PSettingsPage") {
                    static_cast<QWidget*>(c)->setVisible(true);
                } else if (c->objectName().startsWith("P") && c->objectName().endsWith("Page")) {
                    static_cast<QWidget*>(c)->setVisible(false);
                }
            }
        });
    }
    ui->leftMenu->layout()->addWidget(ui->btExit);

    this->initUi();

    connect(ui->btNewProject, &QPushButton::clicked, this, &PHomePage::onNewProject);
    connect(ui->btNewFile, &QPushButton::clicked, this, &PHomePage::onNewFile);
    connect(ui->btOpenFile, &QPushButton::clicked, this, &PHomePage::onOpenFile);
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

PHomePage::~PHomePage()
{
    delete ui;
}

void PHomePage::load()
{
    QSettings settings;

    settings.beginGroup("application/homepage/project");
    auto keys = settings.allKeys();
    keys.sort();
    for (const QString& code: keys) {
        this->addProject(true, settings.value(code).toJsonObject());
    }
    settings.endGroup();

    this->m_files.clear();
    settings.beginGroup("application/homepage/file");
    // TODO SORT BY TITLE
    for (const QString& code: settings.allKeys()) {
        this->addFile(settings.value(code).toJsonObject());
    }
    settings.endGroup();

    this->m_openLastProject = settings.contains("application/homepage/saveproject");
    ui_switchSaveProject->setChecked(this->m_openLastProject);

    this->m_openLastFile = settings.contains("application/homepage/savefile");
    ui_switchSaveFile->setChecked(this->m_openLastFile);

    if (this->m_openLastProject && settings.contains("application/homepage/currentproject"))
    {
        const QString& code = settings.value("application/homepage/currentproject").toString();
        auto projects = ui->projects->findChildren<PHomePageProjectWidget*>();
        for (auto p: projects) {
            if (p->code() == code) {
                p->select();
                this->current_project = p->project();
            }
        }
    }

    if (this->m_openLastFile && settings.contains("application/homepage/currentfile")) {
        const QString& id = settings.value("application/homepage/currentfile").toString();
        for (auto file: this->m_files) {
            if (file->id() == id) {
                this->current_file = file;
                emit this->fileOpened(file);
            }
        }
    }
}

void PHomePage::reload()
{
    auto projects = ui->projects->findChildren<PHomePageProjectWidget*>();
    for (auto project: projects) {
        project->close();
        project->deleteLater();
    }
    auto files = ui->files->findChildren<PHomePageFileWidget*>();
    for (auto file: files) {
        file->close();
        file->deleteLater();
    }
    this->load();

    this->current_project = nullptr;
    QSettings settings;
    settings.remove("application/homepage/currentproject");
    ui->btNewFile->setEnabled(false);
    ui->btOpenFile->setEnabled(false);
}

void PHomePage::initUi()
{
    QWidget* main = parent->centralWidget();
    QWidget* central = main;

    //plugin = this;
    this->setVisible(false);

    if (main->objectName() == "pluginLayout") {
        main->layout()->addWidget(this);
        for (auto c: main->children()) {
            if (c->objectName() == "centralwidget") {
                central = static_cast<QWidget*>(c);
            }
        }

    } else {
        QHBoxLayout* layout = new QHBoxLayout();
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(this);
        layout->addWidget(parent->centralWidget());

        QWidget* global = new QWidget();
        global->setLayout(layout);
        global->setObjectName("pluginLayout");

        parent->setCentralWidget(global);
    }

    parent->centralWidget()->layout()->removeWidget(central);

    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(central);

    QWidget* newcentral = new PCentralWidget(parent, this);
    newcentral->setObjectName("centralwidget");
    newcentral->setLayout(layout);

    parent->centralWidget()->layout()->addWidget(newcentral);
}

void PHomePage::show()
{
    for (auto c: parent->centralWidget()->children()) {
        if (c->objectName() == "centralwidget") {
            static_cast<QWidget*>(c)->setVisible(false);
        } else if (c->objectName() == "PHomePage") {
            static_cast<QWidget*>(c)->setVisible(true);
        } else if (c->objectName().startsWith("P") && c->objectName().endsWith("Page")) {
            static_cast<QWidget*>(c)->setVisible(false);
        }
    }

    /*
    for (auto c: parent->centralWidget().c ->findChildren<QWidget*>()) {
        qDebug() << this << c;
        c->setVisible(false);
        if (c == this) c->setVisible(true);
    }
    */
    //main

    //plugin->setVisible(true);
    //main->setVisible(false);

    if (this->m_openLastFile && this->current_file != nullptr) {
        this->onFileSelected(this->current_file);
    }
}

void PHomePage::hide()
{
    for (auto c: parent->centralWidget()->children()) {
        if (c->objectName() == "centralwidget") {
            static_cast<QWidget*>(c)->setVisible(true);
        } else if (c->objectName().startsWith("P") && c->objectName().endsWith("Page")) {
            static_cast<QWidget*>(c)->setVisible(false);
        }
    }
    //plugin->setVisible(false);
    //main->setVisible(true);
}

void PHomePage::closeCurrentFile()
{
    this->current_file = nullptr;
    if (this->m_openLastFile) {
        QSettings settings;
        settings.remove("application/homepage/currentfile");
    }

    this->show();
}


// PROJECTS //////////////////////////////////////////////////////////////

void PHomePage::addProject(bool hasProject, const QJsonObject& project)
{
    PHomePageProjectWidget* pi = new PHomePageProjectWidget(ui->projects);
    if (hasProject) pi->setProject(project);
    connect(pi, &PHomePageProjectWidget::created, this, &PHomePage::onProjectCreated);
    connect(pi, &PHomePageProjectWidget::selected, this, &PHomePage::onProjectSelected);
    connect(pi, &PHomePageProjectWidget::deleted, this, &PHomePage::onProjectDeleted);
    connect(pi, &PHomePageProjectWidget::updated, this, &PHomePage::onProjectUpdated);
    auto layout = static_cast<QVBoxLayout*>(ui->projects->layout());
    layout->addWidget(pi);
}


void PHomePage::onNewProject()
{
    this->addProject();
}

void PHomePage::onProjectCreated(PHomePageProject* project)
{
    this->reload();

    const QString& code = project->code();
    auto projects = ui->projects->findChildren<PHomePageProjectWidget*>();
    for (auto p: projects) {
        if (p->code() != code) {
            p->unselect();
        } else {
            p->select();
            this->current_project = p->project();
        }
    }
}

void PHomePage::onProjectSelected(PHomePageProject* project)
{
    if (project == nullptr) return;

    auto projects = ui->projects->findChildren<PHomePageProjectWidget*>();
    for (auto p: projects) {
        if (p->code() != project->code()) {
            p->unselect();
        } else {
            this->current_project = project;

            if (this->m_openLastProject) {
                QSettings settings;
                settings.setValue("application/homepage/currentproject", this->current_project->code());
            }
        }
    }

    ui->btNewFile->setEnabled(true);
    ui->btOpenFile->setEnabled(true);

    auto files = ui->files->findChildren<PHomePageFileWidget*>();
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

void PHomePage::onProjectDeleted(PHomePageProject* /*project*/)
{
    this->reload();
}

void PHomePage::onProjectUpdated(PHomePageProject* project)
{
    this->reload();

    const QString& code = project->code();
    auto projects = ui->projects->findChildren<PHomePageProjectWidget*>();
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

void PHomePage::addFile(const QJsonObject& file)
{
    const QString& id = file.value("id").toString();
    const QString& code = file.value("projectcode").toString();
    const QString& title = file.value("title").toString();
    const QString& path = file.value("path").toString();

    auto projects = ui->projects->findChildren<PHomePageProjectWidget*>();
    for (auto p: projects) {
        if (p->code() == code) {
            this->m_files.append(new PHomePageFile(id, p->project(), title, path));
        }
    }
    this->onProjectSelected(this->current_project);
}

void PHomePage::createFile(const QString& title, const QString& path)
{
    const QString& id = QUuid::createUuid().toString(QUuid::WithoutBraces);

    PHomePageFile* file = new PHomePageFile(id, this->current_project, title, path);

    QSettings settings;
    settings.setValue("application/homepage/file/" + id, file->toJson());

    this->m_files.append(file);
    this->onProjectSelected(this->current_project);
}

void PHomePage::addFileToUi(PHomePageFile* file)
{
    PHomePageFileWidget* fw = new PHomePageFileWidget(ui->files);
    fw->setFile(file);
    connect(fw, &PHomePageFileWidget::deleted, this, &PHomePage::onFileDeleted);
    connect(fw, &PHomePageFileWidget::updated, this, &PHomePage::onFileUpdated);
    connect(fw, &PHomePageFileWidget::doubleclicked, this, &PHomePage::onFileSelected);
    auto layout = static_cast<QVBoxLayout*>(ui->files->layout());
    layout->addWidget(fw);
}

void PHomePage::onNewFile()
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

void PHomePage::onOpenFile()
{
    QString fileType = this->m_fileTypeName + " (*." + this->m_fileTypeExtension + ")";

    auto fileName = QFileDialog::getOpenFileName(this, qApp->applicationName(), QDir::homePath(), fileType);
    if (fileName != "")
    {
        const QString& id = QUuid::createUuid().toString(QUuid::WithoutBraces);
        const QString& title = QFileInfo(fileName).baseName();

        PHomePageFile* file = new PHomePageFile(id, this->current_project, title, fileName);

        QSettings settings;
        settings.setValue("application/homepage/file/" + id, file->toJson());

        this->m_files.append(file);
        this->onProjectSelected(this->current_project);
    }
}

void PHomePage::onFileDeleted(PHomePageFile* file)
{
    // TODO Option to delete file on disk
    this->m_files.removeOne(file);
    QSettings settings;
    settings.remove("application/homepage/file/" + file->id());
}

void PHomePage::onFileUpdated(PHomePageFile* file)
{
    QSettings settings;
    settings.setValue("application/homepage/file/" + file->id(), file->toJson());
}

void PHomePage::onFileSelected(PHomePageFile* file)
{
    this->current_file = file;
    if (this->m_openLastFile) {
        QSettings settings;
        settings.setValue("application/homepage/currentfile", file->id());
    }

    this->hide();
    emit this->fileOpened(file);
}



