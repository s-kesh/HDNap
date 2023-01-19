#include "devicepropertymodel.h"

DevicePropertyModel::DevicePropertyModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DevicePropertyModel::rowCount(const QModelIndex &parent) const
{
    return m_items.size();
}

QVariant DevicePropertyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    if (row < 0 || row >= m_items.size())
        return QVariant();

    const auto &item = m_items.at(row);
    switch (role) {
    case NameRole:
        return item.first;
    case ValueRole:
        return item.second.value;
    case MinRole:
        return item.second.min;
    case MaxRole:
        return item.second.max;
    case StepRole:
        return item.second.step;
    case DecimalRole:
        return item.second.decimal;
    default:
        return QVariant();
    }
}

bool DevicePropertyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) //|| role != ValueRole)
        return false;

    int row = index.row();
    if (row < 0 || row >= m_items.size())
        return false;

    switch (role) {
    case ValueRole:
        m_items[row].second.value = value;
        emit dataChanged(index, index, QVector<int>() << ValueRole);
        emit valueChanged(m_items[row].first, m_items[row].second.value);
        break;
    case MinRole:
        m_items[row].second.min = value.toDouble();
        emit dataChanged(index, index, QVector<int>() << MinRole);
        break;
    case MaxRole:
        m_items[row].second.max = value.toDouble();
        emit dataChanged(index, index, QVector<int>() << MaxRole);
        break;
    case StepRole:
        m_items[row].second.step = value.toDouble();
        emit dataChanged(index, index, QVector<int>() << StepRole);
        break;
    case DecimalRole:
        m_items[row].second.decimal = value.toInt();
        emit dataChanged(index, index, QVector<int>() << DecimalRole);
        break;

    }
    return true;
}

Qt::ItemFlags DevicePropertyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}


QHash<int, QByteArray> DevicePropertyModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueRole] = "value";
    roles[MinRole] = "min";
    roles[MaxRole] = "max";
    roles[StepRole] = "step";
    roles[DecimalRole] = "decimal";
    return roles;
}

void DevicePropertyModel::addItem(const QString &name,
                                 const QVariant &value,
                                 double min,
                                 double max, double step, int decimal)
{
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
//    m_items.insert(name, ValueItem{value, min, max, step, decimal});
    m_items.append(qMakePair(name, ValueItem{ value, min, max, step, decimal }));
    endInsertRows();
}
