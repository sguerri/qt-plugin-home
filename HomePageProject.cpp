#include "HomePageProject.h"
#include "ui_HomePageProject.h"

#include <QStyle>
#include <QSettings>
#include <QMouseEvent>
#include <QMessageBox>

#include "utils/Theme.h"
#include "HomePageSlugify.h"

using namespace HomePage;

/*

#include <QRegularExpression>
#include <QMouseEvent>
*/

Project::Project(const QString& code, const QString& title) :
    m_code(code)
  , m_title(title)
{}

QJsonObject Project::toJson() const
{
    QJsonObject obj;
    obj["code"] = this->m_code;
    obj["title"] = this->m_title;
    return obj;
}

Project* Project::fromJson(const QJsonObject& obj)
{
    const QString& code = obj.value("code").toString();
    const QString& title = obj.value("title").toString();

    return new Project(code, title);
}









// EVENT FILTER //////////////////////////////////////////////////////////

bool ProjectWidgetEventFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            emit this->clicked();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}


// CONSTRUCTOR ///////////////////////////////////////////////////////////

ProjectWidget::ProjectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePageProject)
{
    ui->setupUi(this);

    // TODO option to override icons Icon::fromTheme / QIcon::setThemeName
    auto iconCancel = this->style()->standardIcon(QStyle::SP_DialogCancelButton);
    auto iconEdit = this->style()->standardIcon(QStyle::SP_FileIcon);
    auto iconDelete = this->style()->standardIcon(QStyle::SP_TrashIcon);

    ui->btCancel->setIcon(iconCancel);
    ui->btEdit->setIcon(iconEdit);
    ui->btDelete->setIcon(iconDelete);

    connect(ui->txtCode, &QLineEdit::textChanged, this, &ProjectWidget::onCodeChanged);
    connect(ui->btCancel, &QToolButton::clicked, this, &ProjectWidget::onCancel);
    connect(ui->btCreate, &QPushButton::clicked, this, &ProjectWidget::onSave);
    connect(ui->btDelete, &QToolButton::clicked, this, &ProjectWidget::onDelete);
    connect(ui->btEdit, &QToolButton::clicked, this, &ProjectWidget::onEdit);

    ProjectWidgetEventFilter* eventFilter = new ProjectWidgetEventFilter(ui->show);
    connect(eventFilter, &ProjectWidgetEventFilter::clicked, this, &ProjectWidget::select);
    ui->show->installEventFilter(eventFilter);

    this->m_project = nullptr;
    this->m_isEditing = true;
    this->m_isCreating = true;
    this->uiUpdate();
}

ProjectWidget::~ProjectWidget()
{
    delete ui;
}


// UI ////////////////////////////////////////////////////////////////////

void ProjectWidget::uiUpdate()
{
    if (this->hasProject()) {
        ui->lblCode->setText(this->m_project->code());
        ui->lblTitle->setText(this->m_project->title());
        ui->txtCode->setText(this->m_project->code());
        ui->txtTitle->setText(this->m_project->title());
    } else {
        ui->lblCode->setText("");
        ui->lblTitle->setText("");
        ui->txtCode->setText("");
        ui->txtTitle->setText("");
    }

    ui->txtCode->setEnabled(this->m_isCreating);
    ui->btCreate->setText(this->m_isCreating ? tr("Create the project") : tr("Save changes"));

    ui->show->setVisible(!this->m_isEditing);
    ui->edit->setVisible(this->m_isEditing);
    if (this->m_isEditing && this->m_isCreating) ui->txtCode->setFocus();
    if (this->m_isEditing && !this->m_isCreating) ui->txtTitle->setFocus();
}


// SLOTS /////////////////////////////////////////////////////////////////

void ProjectWidget::onCancel()
{
    if (this->m_isCreating) {
        this->close();
        this->deleteLater();
    } else {
        this->m_isEditing = false;
        this->uiUpdate();
    }
}

void ProjectWidget::onSave()
{
    if (this->m_isCreating)
    {
        if (ui->txtCode->text() == "" || ui->txtCode->text() == "-") return; // TODO add notification
        if (ui->txtTitle->text().trimmed() == "") return; // TODO add notification

        QString code = ui->txtCode->text();
        QString title = ui->txtTitle->text().trimmed();

        QSettings settings;
        if (settings.contains("homepage/project/" + code)) return; // TODO add notification

        this->m_project = new Project(code, title);
        this->m_isCreating = false;
        this->m_isEditing = false;
        this->uiUpdate();

        settings.setValue("homepage/project/" + code, this->m_project->toJson());

        emit this->created(this->m_project);
    }
    else
    {
        if (ui->txtTitle->text().trimmed() == "") return; // TODO add notification

        QString title = ui->txtTitle->text().trimmed();

        this->m_project->setTitle(title);
        this->m_isCreating = false;
        this->m_isEditing = false;
        this->uiUpdate();

        QSettings settings;
        settings.setValue("homepage/project/" + this->m_project->code(), this->m_project->toJson());

        emit this->updated(this->m_project);
    }
}

void ProjectWidget::onDelete()
{
    if (QMessageBox::question(this, qApp->applicationName(), tr("Delete project?")) == QMessageBox::Yes) {
        QSettings settings;
        settings.remove("homepage/project/" + this->m_project->code());

        emit this->deleted(this->m_project);

        this->close();
        this->deleteLater();
    }
}

void ProjectWidget::onEdit()
{
    this->m_isEditing = true;
    this->m_isCreating = false;
    this->uiUpdate();
}

void ProjectWidget::onCodeChanged(const QString& text)
{
    int pos = ui->txtCode->cursorPosition();
    ui->txtCode->setText(HomePage::slugify(text));
    ui->txtCode->setCursorPosition(pos);
}


// ACTIONS ///////////////////////////////////////////////////////////////

void ProjectWidget::select()
{
    ui->show->setStyleSheet("background-color: " + Theme::getColor("@homepageprojetselected"));
    emit this->selected(this->m_project);
}

void ProjectWidget::unselect()
{
    ui->show->setStyleSheet("background-color: " + Theme::getColor("@homepageprojetunselected"));
}

