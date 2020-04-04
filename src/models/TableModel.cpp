#include "TableModel.h"

#include <QColor>

#include <cmath>

void TableModel::setTable(lt::Table && table) noexcept
{
    beginResetModel();
    table_ = std::move(table);
    endResetModel();
}

int TableModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;

    return table_.height();
}

int TableModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return 0;

    return table_.width();
}

QVariant TableModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::UserRole && role != Qt::DisplayRole && role != Qt::BackgroundColorRole &&
        role != Qt::ForegroundRole)
        return QVariant();

    if (index.row() < 0 || index.row() >= table_.height() || index.column() < 0 || index.column() >= table_.width())
        return QVariant();

    if (role == Qt::DisplayRole)
        return table_.get(index.row(), index.column());

    if (role == Qt::ForegroundRole)
    {
        if (table_.isScalar())
            return QVariant();

        return QColor(0, 0, 0);
    }

    if (role == Qt::BackgroundColorRole)
    {
        if (table_.isScalar())
            return QVariant();

        double diff = table_.maximum() - table_.minimum();
        if (diff == 0.0)
            return QColor::fromHsvF((1.0 / 3.0), 1.0, 1.0);
        double ratio = static_cast<double>(table_.get(index.row(), index.column()) - table_.minimum()) / diff;
        ratio = std::clamp(ratio, 0.0, 1.0);
        return QColor::fromHsvF((1.0 - ratio) * (1.0 / 3.0), 1.0, 1.0);
    }

    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    lt::AxisPtr axis;
    if (orientation == Qt::Horizontal)
        axis = table_.xAxis();
    else if (orientation == Qt::Vertical)
        axis = table_.yAxis();

    if (!axis)
        return QVariant();

    return QString::number(std::floor(axis->index(section) * 100.0) / 100.0);
}

bool TableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (!index.isValid())
        return false;

    if (role != Qt::EditRole)
        return false;

    if (index.row() < 0 || index.row() >= table_.height() || index.column() < 0 || index.column() >= table_.width())
        return false;

    bool ok;
    double val = value.toDouble(&ok);
    if (!ok)
        return false;

    table_.set(index.row(), index.column(), val);
    emit dataChanged(index, index);
    return true;
}

Qt::ItemFlags TableModel::flags(const QModelIndex & index) const
{
    if (index.isValid())
    {
        return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
    }
    return Qt::NoItemFlags;
}

void TableModel::scaleAll(double scale)
{
    for (int r = 0; r < table_.height(); ++r)
    {
        for (int c = 0; c < table_.width(); ++c)
        {
            table_.set(r, c, table_.get(r, c) * scale);
        }
    }

    emit dataChanged(index(0, 0), index(table_.height() - 1, table_.width() - 1));
}

void TableModel::addAll(double amount)
{
    for (int r = 0; r < table_.height(); ++r)
    {
        for (int c = 0; c < table_.width(); ++c)
        {
            table_.set(r, c, table_.get(r, c) + amount);
        }
    }

    emit dataChanged(index(0, 0), index(table_.height() - 1, table_.width() - 1));
}
