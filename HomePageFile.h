#ifndef HOMEPAGEFILE_H
#define HOMEPAGEFILE_H

//#include "data/File.h"

#include "HomePageProject.h"

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class HomePageFile;
}

namespace HomePage
{

    class File
    {
    public:
        explicit File(const QString& id, Project* project, const QString& title, const QString& path);

        QString id() const { return this->m_id; }

        QString title() const { return this->m_title; }
        void setTitle(const QString& value) { this->m_title = value; }

        QString path() const { return this->m_path; }
        void setPath(const QString& value) { this->m_path = value; }

        Project* project() { return this->m_project; }
        void setProject(Project* value) { this->m_project = value; }

        QJsonObject toJson() const;
        //static File* fromJson(Project* project, const QJsonObject& obj);

    private:
        QString m_id;
        Project* m_project;
        QString m_title;
        QString m_path;
    };


    class FileWidgetEventFilter : public QObject
    {
        Q_OBJECT
    public:
        explicit FileWidgetEventFilter(QWidget* parent) : QObject(parent) {}

        bool eventFilter(QObject* obj, QEvent* event);

    signals:
        void doubleclicked();
    };




    class FileWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit FileWidget(QWidget* parent = nullptr);
        ~FileWidget();

        bool hasFile() const { return this->m_file != nullptr; }
        void setFile(File* file) { this->m_file = file; this->m_isEditing = false; this->uiUpdate(); }

    private:
        Ui::HomePageFile* ui;

        File* m_file;
        bool m_isEditing;

    private:
        void uiUpdate();

    private slots:
        void onDelete();
        void onEdit();
        void onCancel();
        void onUpdate();

    signals:
        void deleted(HomePage::File* file);
        void updated(HomePage::File* file);
        void doubleclicked(HomePage::File* file);
    };

}

#endif // HOMEPAGEFILE_H
