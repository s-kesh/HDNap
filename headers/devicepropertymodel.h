#ifndef DEVICEPROPERTYMODEL_H
#define DEVICEPROPERTYMODEL_H

/* It is DataModel Class
 * It inherent from QAbstractListModel so that
 * it can be used as a List Model in QML (out GUI)
 * It is basically host a list with following elements
 * <Name> <Value> <Minimum> <Maximum> <Stepsize> and <No of Decimals>
 * Let say our device has some parameter like width
 * which can vary from 1 to 1024 in step of 4.
 * It has value 512 presently
 * then we need to add item to our list like
 * addItem("Width", 512, 1, 1024, 4, 0)
 * Decimals are 0 as it is an integer.
 */

#include <QAbstractListModel>
#include <QStringList>
#include <QMap>
#include <QVariant>

class DevicePropertyModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        NameRole = Qt::UserRole + 1,
        ValueRole,
        MinRole,
        MaxRole,
        StepRole,
        DecimalRole
    };

    DevicePropertyModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QHash<int, QByteArray> roleNames() const override;
    void addItem(const QString &name,
                 const QVariant &value,
                 double min,
                 double max,
                 double step,
                 int decimal);

public slots:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

signals:
    void valueChanged(QString name, QVariant value);


private:
    struct ValueItem {
        QVariant value;
        double min;
        double max;
        double step;
        int decimal;
    };

    QList<QPair<QString, ValueItem>> m_items;
};

#endif // DEVICEPROPERTYMODEL_H
