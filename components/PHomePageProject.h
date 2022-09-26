#ifndef PHOMEPAGEPROJECT_H
#define PHOMEPAGEPROJECT_H

//#include "data/Project.h"

#include <QWidget>
#include <QJsonObject>

namespace Ui {
    class PHomePageProject;
}

class PHomePageProject
{
public:
    explicit PHomePageProject(const QString& code, const QString& title);

    QString code() const { return this->m_code; }

    QString title() const { return this->m_title; }
    void setTitle(const QString& value) { this->m_title = value; }

    QJsonObject toJson() const;
    static PHomePageProject* fromJson(const QJsonObject& obj);

private:
    QString m_code;
    QString m_title;
};



class PHomePageProjectWidgetEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit PHomePageProjectWidgetEventFilter(QWidget* parent) : QObject(parent) {}

    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void clicked();
};


class PHomePageProjectWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PHomePageProjectWidget(QWidget* parent = nullptr);
    ~PHomePageProjectWidget();

    PHomePageProject* project() { return this->m_project; }
    bool hasProject() const { return this->m_project != nullptr; }
    void setProject(const QJsonObject& obj) { this->m_project = PHomePageProject::fromJson(obj); this->m_isEditing = false; this->uiUpdate(); }
    QString code() const { return this->hasProject() ? this->m_project->code() : ""; }
    QString title() const { return this->hasProject() ? this->m_project->title() : ""; }

    void select();
    void unselect();

private:
    Ui::PHomePageProject* ui;

    PHomePageProject* m_project;

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
    void created(PHomePageProject* project);
    void selected(PHomePageProject* project);
    void deleted(PHomePageProject* project);
    void updated(PHomePageProject* project);
};

#endif // PHOMEPAGEPROJECT_H
