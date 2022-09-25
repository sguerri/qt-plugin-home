#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "HomePageProject.h"
#include "HomePageFile.h"

#include "components/QSwitchButton.h"

#include <QWidget>
#include <QMainWindow>
#include <QJsonObject>

namespace Ui {
class HomePage;
}

namespace HomePage
{
    class Plugin : public QWidget
    {
        Q_OBJECT

    public:
        explicit Plugin(QMainWindow* parent);
        ~Plugin();

        void show();
        void closeCurrentFile();

        bool hasCurrentProject() const { return this->current_project != nullptr; }
        Project* currentProject() { return this->current_project; }

        bool hasCurrentFile() const { return this->current_file != nullptr; }
        File* currentFile() { return this->current_file; }

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
        Ui::HomePage* ui;
        QSwitchButton* ui_switchSaveProject;
        QSwitchButton* ui_switchSaveFile;

        QMainWindow* parent;
        QWidget* home;
        QWidget* main;

        void hide();
        void initUi();
        void load();
        void reload();

        void addProject(bool hasProject = false, const QJsonObject& project = QJsonObject());
        void addFile(const QJsonObject& file);
        void createFile(const QString& title, const QString& path);
        void addFileToUi(File* file);

        Project* current_project;
        File* current_file;
        QList<File*> m_files;

        bool m_openLastProject;
        bool m_openLastFile;
        QString m_fileTypeExtension;
        QString m_fileTypeName;
        QString m_fileDefault;

    private slots:
        void onNewProject();
        void onNewFile();
        void onOpenFile();

        void onProjectCreated(HomePage::Project* project);
        void onProjectSelected(HomePage::Project* project);
        void onProjectDeleted(HomePage::Project* project);
        void onProjectUpdated(HomePage::Project* project);

        void onFileDeleted(HomePage::File* file);
        void onFileUpdated(HomePage::File* file);
        void onFileSelected(HomePage::File* file);

    signals:
        void fileOpened(HomePage::File* file);
    };

}


#endif // HOMEPAGE_H
