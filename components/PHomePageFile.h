#ifndef PHOMEPAGEFILE_H
#define PHOMEPAGEFILE_H

//#include "data/File.h"

#include "PHomePageProject.h"

#include <QWidget>
#include <QJsonObject>

namespace Ui {
    class PHomePageFile;
}

class PHomePageFile
{
public:
    explicit PHomePageFile(const QString& id, PHomePageProject* project, const QString& title, const QString& path);

    QString id() const { return this->m_id; }

    QString title() const { return this->m_title; }
    void setTitle(const QString& value) { this->m_title = value; }

    QString path() const { return this->m_path; }
    void setPath(const QString& value) { this->m_path = value; }

    PHomePageProject* project() { return this->m_project; }
    void setProject(PHomePageProject* value) { this->m_project = value; }

    QJsonObject toJson() const;
    //static File* fromJson(Project* project, const QJsonObject& obj);

private:
    QString m_id;
    PHomePageProject* m_project;
    QString m_title;
    QString m_path;
};


class PHomePageFileWidgetEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit PHomePageFileWidgetEventFilter(QWidget* parent) : QObject(parent) {}

    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void doubleclicked();
};




class PHomePageFileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PHomePageFileWidget(QWidget* parent = nullptr);
    ~PHomePageFileWidget();

    bool hasFile() const { return this->m_file != nullptr; }
    void setFile(PHomePageFile* file) { this->m_file = file; this->m_isEditing = false; this->uiUpdate(); }

private:
    Ui::PHomePageFile* ui;

    PHomePageFile* m_file;
    bool m_isEditing;

private:
    void uiUpdate();

private slots:
    void onDelete();
    void onEdit();
    void onCancel();
    void onUpdate();

signals:
    void deleted(PHomePageFile* file);
    void updated(PHomePageFile* file);
    void doubleclicked(PHomePageFile* file);
};

#endif // PHOMEPAGEFILE_H
