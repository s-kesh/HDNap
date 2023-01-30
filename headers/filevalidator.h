#ifndef FILEVALIDATOR_H
#define FILEVALIDATOR_H

#include "qobject.h"
#include <QObject>

class FileValidator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isExist READ isExist WRITE setIsExist NOTIFY isExistChanged)
    Q_PROPERTY(QString filename READ filename WRITE setFilename NOTIFY filenameChanged)
    Q_PROPERTY(QString folderPath READ folderPath WRITE setFolderPath NOTIFY folderPathChanged)


public:
    explicit FileValidator(QObject *parent = nullptr);

    bool isExist() const;
    void setIsExist(bool newIsExist);

    const QString &filename() const;
    void setFilename(const QString &newFilename);

    const QString &folderPath() const;
    void setFolderPath(const QString &newFolderPath);

signals:
    void isExistChanged();

    void filenameChanged();

    void folderPathChanged();

private:
    bool m_isExist = false;
    QString m_filename;
    QString m_folderPath;
};

#endif // FILEVALIDATOR_H
