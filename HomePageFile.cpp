#include "HomePageFile.h"
#include "ui_HomePageFile.h"

#include <QStyle>
#include <QMessageBox>

using namespace HomePage;

File::File(const QString& id, Project* project, const QString& title, const QString& path) :
    m_id(id)
  , m_project(project)
  , m_title(title)
  , m_path(path)
{}

QJsonObject File::toJson() const
{
    QJsonObject obj;
    obj["id"] = this->m_id;
    obj["projectcode"] = this->m_project->code();
    obj["title"] = this->m_title;
    obj["path"] = this->m_path;
    return obj;
}
/*
File* File::fromJson(Project* project, const QJsonObject& obj)
{
    const QString& id = obj.value("id").toString();
    const QString& title = obj.value("title").toString();
    const QString& path = obj.value("path").toString();
    return new File(id, project, title, path);
}
*/


// EVENT FILTER //////////////////////////////////////////////////////////

bool FileWidgetEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        emit this->doubleclicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}


// CONSTRUCTOR ///////////////////////////////////////////////////////////

FileWidget::FileWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePageFile)
{
    ui->setupUi(this);

    // TODO option to override icons Icon::fromTheme / QIcon::setThemeName
    auto iconCancel = this->style()->standardIcon(QStyle::SP_DialogCancelButton);
    auto iconEdit = this->style()->standardIcon(QStyle::SP_FileIcon);
    auto iconDelete = this->style()->standardIcon(QStyle::SP_TrashIcon);

    ui->btCancel->setIcon(iconCancel);
    ui->btEdit->setIcon(iconEdit);
    ui->btDelete->setIcon(iconDelete);

    connect(ui->btDelete, &QToolButton::clicked, this, &FileWidget::onDelete);
    connect(ui->btEdit, &QToolButton::clicked, this, &FileWidget::onEdit);
    connect(ui->btCancel, &QToolButton::clicked, this, &FileWidget::onCancel);
    connect(ui->btUpdate, &QPushButton::clicked, this, &FileWidget::onUpdate);

    FileWidgetEventFilter* eventFilter = new FileWidgetEventFilter(ui->show);
    connect(eventFilter, &FileWidgetEventFilter::doubleclicked, this, [=](){
        emit this->doubleclicked(this->m_file);
    });
    ui->show->installEventFilter(eventFilter);

    this->m_file = nullptr;
    this->m_isEditing = false;
    this->uiUpdate();
}

FileWidget::~FileWidget()
{
    delete ui;
}


// UI ////////////////////////////////////////////////////////////////////

void FileWidget::uiUpdate()
{
    if (this->hasFile()) {
        ui->lblTitle->setText(this->m_file->title());
        ui->lblPath->setText(this->m_file->path());
        ui->txtTitle->setText(this->m_file->title());
        ui->txtPath->setText(this->m_file->path());
    } else {
        ui->lblTitle->setText("");
        ui->lblPath->setText("");
        ui->txtTitle->setText("");
        ui->txtPath->setText("");
    }

    ui->show->setVisible(!this->m_isEditing);
    ui->edit->setVisible(this->m_isEditing);
    ui->txtPath->setEnabled(!this->m_isEditing);
    if (this->m_isEditing) ui->txtTitle->setFocus();
}

void FileWidget::onDelete()
{
    if (QMessageBox::question(this, qApp->applicationName(), tr("Delete file?")) == QMessageBox::Yes) {
        emit this->deleted(this->m_file);
        this->close();
        this->deleteLater();
    }
}

void FileWidget::onEdit()
{
    this->m_isEditing = true;
    this->uiUpdate();
}

void FileWidget::onCancel()
{
    this->m_isEditing = false;
    this->uiUpdate();
}

void FileWidget::onUpdate()
{
    if (ui->txtTitle->text().trimmed() == "") return; // TODO notification

    this->m_file->setTitle(ui->txtTitle->text().trimmed());
    this->m_isEditing = false;
    this->uiUpdate();

    emit this->updated(this->m_file);
}

