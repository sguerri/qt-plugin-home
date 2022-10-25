#ifndef PHOMEPAGE_H
#define PHOMEPAGE_H

#include "CSwitchButton.h"

#include "components/PHomePageProject.h"
#include "components/PHomePageFile.h"

#include <QWidget>
#include <QMainWindow>
#include <QJsonObject>

namespace Ui {
    class PHomePage;
}

class PHomePage;

class PCentralWidget: public QWidget
{
    Q_OBJECT

public:
    explicit PCentralWidget(QMainWindow* parent, PHomePage* page);
    ~PCentralWidget();

private:
    PHomePage* m_page;

protected:
    void showEvent(QShowEvent* event);
};

class PHomePage : public QWidget
{
    Q_OBJECT

public:
    explicit PHomePage(QMainWindow* parent, std::function<void (PHomePageFile*)> fn);
    ~PHomePage();

    void show();
    void hide();

    void closeCurrentFile();

    bool hasCurrentProject() const { return this->current_project != nullptr; }
    PHomePageProject* currentProject() { return this->current_project; }

    bool hasCurrentFile() const { return this->current_file != nullptr; }
    PHomePageFile* currentFile() { return this->current_file; }

    /*
    bool hasCurrentProjectAutosave() const { return this->m_openLastProject; }
    void setCurrentProjectAutosave(bool value) { this->m_openLastProject = value; }
    void setAutosave(bool value) { this->setCurrentProjectAutosave(value); }
    */

    QPair<QString, QString> fileType() const { return { this->m_fileTypeExtension, this->m_fileTypeName }; }
    void setFileType(const QString& extension, const QString& name) { this->m_fileTypeExtension = extension; this->m_fileTypeName = name; }

    QString fileDefault() const { return this->m_fileDefault; }
    void setFileDefault(const QString& value) { this->m_fileDefault = value; }

private:
    Ui::PHomePage* ui;
    CSwitchButton* ui_switchSaveProject;
    CSwitchButton* ui_switchSaveFile;

    QMainWindow* parent;
    //QWidget* plugin;
    //QWidget* main;

    void initUi();
    void load();
    void reload();

    void addProject(bool hasProject = false, const QJsonObject& project = QJsonObject());
    void addFile(const QJsonObject& file);
    void createFile(const QString& title, const QString& path);
    void addFileToUi(PHomePageFile* file);

    PHomePageProject* current_project;
    PHomePageFile* current_file;
    QList<PHomePageFile*> m_files;

    bool m_openLastProject;
    bool m_openLastFile;
    QString m_fileTypeExtension;
    QString m_fileTypeName;
    QString m_fileDefault;

private slots:
    void onNewProject();
    void onNewFile();
    void onOpenFile();

    void onProjectCreated(PHomePageProject* project);
    void onProjectSelected(PHomePageProject* project);
    void onProjectDeleted(PHomePageProject* project);
    void onProjectUpdated(PHomePageProject* project);

    void onFileDeleted(PHomePageFile* file);
    void onFileUpdated(PHomePageFile* file);
    void onFileSelected(PHomePageFile* file);

signals:
    void fileOpened(PHomePageFile* file);
};

#endif // PHOMEPAGE_H
