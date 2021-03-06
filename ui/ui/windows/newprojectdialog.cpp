#include "newprojectdialog.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

#include <QStyle>

NewProjectDialog::NewProjectDialog(QWidget * parent) : QDialog(parent)
{
    setWindowTitle("Create new Project");
    resize(600, 200);
    lineName_ = new QLineEdit;
    linePath_ = new QLineEdit(QDir(QStandardPaths::writableLocation(
                                       QStandardPaths::DocumentsLocation))
                                  .filePath("LibreTuner Projects"));
    linePath_->setClearButtonEnabled(true);

    auto * buttonCreate = new QPushButton(tr("Create"));
    buttonCreate->setDefault(true);
    buttonCreate->setAutoDefault(true);
    auto * buttonCancel = new QPushButton(tr("Cancel"));

    auto * buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(buttonCreate);
    buttonLayout->addWidget(buttonCancel);

    auto * buttonBrowse = new QPushButton(
        style()->standardIcon(QStyle::SP_DirOpenIcon), QString());
    auto * pathLayout = new QHBoxLayout;
    pathLayout->addWidget(linePath_);
    pathLayout->addWidget(buttonBrowse);

    auto * form = new QFormLayout;
    form->addRow(tr("Project Name:"), lineName_);
    form->addRow(tr("Location:"), pathLayout);

    auto * propertiesGroup = new QGroupBox(tr("Properties"));
    propertiesGroup->setLayout(form);

    auto * layout = new QVBoxLayout;
    layout->addWidget(propertiesGroup);
    layout->addLayout(buttonLayout);
    setLayout(layout);

    connect(buttonCancel, &QPushButton::clicked, this,
            &NewProjectDialog::reject);

    connect(buttonBrowse, &QPushButton::clicked, [this]() {
        QString dir = QFileDialog::getExistingDirectory(
            nullptr, tr("Select Project Directory - LibreTuner"),
            linePath_->text());
        if (!dir.isNull())
            linePath_->setText(dir);
    });

    connect(buttonCreate, &QPushButton::clicked, [this]() {
        if (lineName_->text().trimmed().isEmpty())
        {
            QMessageBox::warning(nullptr, tr("Invalid Name"),
                                 tr("The chosen directory already exists."));
            return;
        }
        QDir dir = QDir(path());
        if (dir.exists())
        {
            QMessageBox msg(
                QMessageBox::Question, tr("Directory Exists"),
                tr("A project with that path already exists, would you "
                   "like to load or overwrite it?"),
                QMessageBox::Cancel);
            auto * buttonLoad =
                msg.addButton(tr("Load"), QMessageBox::AcceptRole);
            auto * buttonOverwrite =
                msg.addButton(tr("Overwrite"), QMessageBox::ResetRole);
            msg.exec();

            if (msg.clickedButton() == buttonLoad)
                openProject_ = true;
            else if (msg.clickedButton() != buttonOverwrite)
                return; // "No" was clicked; do nothing.
        }
        else
            openProject_ = false;

        accept();
    });
}

QString NewProjectDialog::path() const
{
    return QDir(linePath_->text()).filePath(lineName_->text());
}
