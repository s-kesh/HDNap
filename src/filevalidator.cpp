#include "filevalidator.h"
#include <QDir>
#include <QFileInfo>

FileValidator::FileValidator(QObject *parent)
{

}

bool FileValidator::isExist() const
{
    return m_isExist;
}

void FileValidator::setIsExist(bool newIsExist)
{
    if (m_isExist == newIsExist)
        return;
    m_isExist = newIsExist;
}

const QString &FileValidator::filename() const
{
    return m_filename;
}

void FileValidator::setFilename(const QString &newFilename)
{
    m_filename = newFilename;

    if (QFileInfo::exists(m_filename + "_tof.bin") ||
            QFileInfo::exists(m_filename + "_image.bin") ) {
        m_isExist = true;
    }
    else {
        m_isExist = false;
    }
}

const QString &FileValidator::folderPath() const
{
    return m_folderPath;
}

void FileValidator::setFolderPath(const QString &newFolderPath)
{
    if (m_folderPath == newFolderPath)
        return;
    m_folderPath = newFolderPath;

    QDir directory(m_folderPath);
    if (!directory.exists())
        directory.mkpath(".");
}
