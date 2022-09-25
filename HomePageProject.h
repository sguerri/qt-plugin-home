#ifndef HOMEPAGEPROJECT_H
#define HOMEPAGEPROJECT_H

//#include "data/Project.h"

#include <QWidget>
#include <QJsonObject>

namespace Ui {
class HomePageProject;
}

namespace HomePage
{


    class Project
    {
    public:
        explicit Project(const QString& code, const QString& title);

        QString code() const { return this->m_code; }

        QString title() const { return this->m_title; }
        void setTitle(const QString& value) { this->m_title = value; }

        QJsonObject toJson() const;
        static Project* fromJson(const QJsonObject& obj);

    private:
        QString m_code;
        QString m_title;
    };



    class ProjectWidgetEventFilter : public QObject
    {
        Q_OBJECT
    public:
        explicit ProjectWidgetEventFilter(QWidget* parent) : QObject(parent) {}

        bool eventFilter(QObject* obj, QEvent* event);

    signals:
        void clicked();
    };


    class ProjectWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit ProjectWidget(QWidget* parent = nullptr);
        ~ProjectWidget();

        Project* project() { return this->m_project; }
        bool hasProject() const { return this->m_project != nullptr; }
        void setProject(const QJsonObject& obj) { this->m_project = Project::fromJson(obj); this->m_isEditing = false; this->uiUpdate(); }
        QString code() const { return this->hasProject() ? this->m_project->code() : ""; }
        QString title() const { return this->hasProject() ? this->m_project->title() : ""; }

        void select();
        void unselect();

    private:
        Ui::HomePageProject* ui;

        Project* m_project;

        bool m_isEditing;
        bool m_isCreating;

    private:
        void uiUpdate();

    private slots:
        void onCancel();
        void onSave();
        void onDelete();
        void onEdit();
        void onCodeChanged(const QString& text);

    signals:
        void created(HomePage::Project* project);
        void selected(HomePage::Project* project);
        void deleted(HomePage::Project* project);
        void updated(HomePage::Project* project);
    };

}

#endif // HOMEPAGEPROJECT_H
