/*
 * LibreTuner
 * Copyright (C) 2018 Altenius
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dataloggerwindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QShowEvent>
#include <QVBoxLayout>

#include "datalink.h"
#include "datalog.h"
#include "datalogger.h"
#include "definitions/definition.h"
#include "libretuner.h"

DataLoggerWindow::DataLoggerWindow(const DataLogPtr &log, std::unique_ptr<DataLogger> &&logger, const definition::MainPtr &definition, QWidget *parent) : StyledWindow(parent), log_(log), logger_(std::move(logger)), definition_(definition)
{
    Expects(logger_);
    Expects(log_);
    Expects(definition_);
    setAttribute( Qt::WA_DeleteOnClose, false );
    setTitle("LibreTuner - Data Logger");

    QWidget *main = new QWidget();

    auto *hlayout = new QHBoxLayout;
    mainLayout()->addWidget(main);
    main->setLayout(hlayout);

    auto *pidLayout = new QVBoxLayout;
    hlayout->addLayout(pidLayout);

    QLabel *pidLabel = new QLabel("Available PIDs");
    pidLayout->addWidget(pidLabel);

    pidList_ = new QListWidget;
    pidLayout->addWidget(pidList_);

    auto *logLayout = new QVBoxLayout;
    hlayout->addLayout(logLayout);
    logOutput_ = new QTreeWidget;
    logOutput_->setHeaderHidden(true);
    logLayout->addWidget(logOutput_);

    buttonLog_ = new QPushButton("Start logging");
    logLayout->addWidget(buttonLog_);
    connect(buttonLog_, &QPushButton::clicked, this,
            &DataLoggerWindow::buttonClicked);
    reset();
    
    connection_ = log_->connectUpdate([this] (const DataLog::Data& info, double value) {
        logUpdate(info, value);
    });
}

void DataLoggerWindow::logUpdate(const DataLog::Data& info, double value)
{
    Logger::debug("Update to " + std::to_string(value));
}


DataLoggerWindow::~DataLoggerWindow() = default;

void DataLoggerWindow::showEvent(QShowEvent *event) {
    Q_UNUSED(event)

}

void DataLoggerWindow::hideEvent(QHideEvent * /*event*/) {
    // TODO: ask to save log
    pidList_->clear();
    definition_.reset();
    log_.reset();
    logger_.reset();
}

void DataLoggerWindow::buttonClicked() {
    if (!logger_) {
        return;
    }

    if (!logger_->running()) {
        logger_->enable();
        buttonLog_->setText("Stop logging");
    } else {
        logger_->disable();
        buttonLog_->setText("Start logging");
    }
}

void DataLoggerWindow::reset()
{
    pidList_->clear();
    logOutput_->clear();

    // TODO: set error handler
    logger_->setLog(log_);

    for (const PidDefinition &pid : definition_->pids().pids()) {
        if (!pid.valid) {
            continue;
        }
        try {
            log_->addData({pid.name, pid.description, pid.id, DataUnit::None});
            logger_->addPid(pid.id, pid.code, pid.formula);
        } catch (const std::exception &e) {
            Logger::warning(std::string("Error while adding PID ") + std::to_string(pid.id) + ": " + e.what());
            continue;
        }
        auto *item = new QListWidgetItem;
        item->setText(QString::fromStdString(pid.name));
        item->setData(Qt::UserRole, QVariant::fromValue<uint32_t>(pid.id));
        item->setFlags(Qt::ItemIsUserCheckable);
        pidList_->addItem(item);
    }
}
