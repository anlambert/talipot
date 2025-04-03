/**
 *
 * Copyright (C) 2019-2025  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PROJECT_H
#define TALIPOT_PROJECT_H

#include <QDir>
#include <QString>
#include <QTemporaryDir>

#include <talipot/config.h>

#include <fstream>

namespace tlp {

class PluginProgress;

/**
  @ingroup Plugins

  @brief The Project object handles the content of a Talipot project.

  All talipot projects contain a set of defined static meta-information:
  @list
  @li name (QString): the name of the project
  @li description (QString): Comments about the project
  @li author (QString): the author of the project
  @li date (QDate): the date of project's last modification
  @li version (QString): the version of the Talipot project format
  @endlist

  A Project DOES NOT automatically save to disk. One will have to call the write() method to
  serialize data.
  @warning Precise implementation of the Project object should NOT be known or used by the user
  since it could be subject to changes.

  If something wrong happens when calling a method from Project, this method will return either
  false or a invalid result (see specific method documentation). The last error message can be
  retrieved with the lastError() method.

  After opening and before saving a project, user will be able to list/delete files and directories
  available in the project and open them using std filestreams or Qt's QIODevice.
  Files can be opened using the stdFileStram and fileStream methods. They will always be opened in
  Read/Write mode.

  Files in a talipot project are identified by their path. Those path ar similar to the path of a
  standard file system and use the "/" character as a separator.
  The root directory is identified by "/".

  @warning Project path ALWAYS use the "/" character as a directory separator. This is
  OS-independant.

  @note A file called graph.tlp located at the top-level directory will be identified by the
  "/graph.tlp" path while the file "graph.tlp" located in the "data" directory will be identified by
  "/data/graph.tlp".
  */
class TLP_QT_SCOPE Project : public QObject {
    Q_OBJECT

    Project() = delete;
    explicit Project(QTemporaryDir *);

  public:
    ~Project() override;

    /**
      @brief Starts a new Project from scratch

      This method builds up a new Project file without taking any input.
      @see openProject()
      */
    static Project *newProject();

    /**
      @brief Opens a previously saved talipot project file and returns the corresponding project

      This method will unpack a talipot project file into some directory and allow the user to
      manipulate the files.
      @see Project::save()
      @param file The file to open.
      @param progress A progress handler.
      @return a pointer to a Project object.
      */
    static Project *openProject(const QString &file, tlp::PluginProgress *progress = nullptr);

    /**
      @brief Opens a previously saved talipot project file

      This method unpacks a talipot project file into some directory and allow the user to
      manipulate the files.
      @see Project::save()
      @param file The file to open.
      @param progress A progress handler.
      @return true if the file has been successfully opened
      */
    bool openProjectFile(const QString &file, tlp::PluginProgress *progress = nullptr);

    /*

        @brief Restores a project which has already been extracted into path

        @warning Using several Project instances on the same directory may result in undefined
        behavior. This method should only be used for crash handling purposes.
        @param path The path where the archive was previously extracted
        @return a pointer to a Project object.

      static Project *restoreProject(const QString &path); */

    /**
     * @brief Removes all files in the project and unset project file if any
     *
     */
    bool clearProject();

    /**
     * @brief Sets the file where to save the project
     * @param projectFile absolute path to a .tlpx file
     *
     */
    void setProjectFile(const QString &projectFile);

    /**
      @brief Writes files in the Project into a packed archive.

      This method packs every file in the project into a single archive.
      @note This method DOES NOT close the project. It only  commits changes to the specified file.
      A Project is only closed when destroyed.
      @param file Absolute path where files should be packed.
      @param progress A progress handler
      @return False if method failed
      */
    bool write(const QString &file, tlp::PluginProgress *progress = nullptr);

    /**
      @brief Lists entries in a directory

      @see QDir documentation for a complete description of filtering arguments
      @param path The path to scan. @see Project
      @return The list of files and directories present in the given directory
      */
    QStringList entryList(const QString &path, QDir::Filters filters = QDir::NoFilter,
                          QDir::SortFlags sort = QDir::NoSort);

    /**
      @brief Lists entries in a directory

      @see QDir documentation for a complete description of filtering arguments
      @param path The path to scan. @see Project
      @return The list of files and directories present in the given directory
      */
    QStringList entryList(const QString &path, const QStringList &nameFilters,
                          QDir::Filters filters = QDir::NoFilter,
                          QDir::SortFlags sort = QDir::NoSort);

    /**
      @brief Checks if the specified file/folder exists

      @param path The path to check.
      @return true if the path exists.
      */
    bool exists(const QString &path);

    /**
      @brief Recursively creates the specified path.

      Created folders will be empty
      @return true if path was successfully created.
      */
    bool mkpath(const QString &path);

    /**
      @brief Checks if the given path is a directory.

      @param path The path to check. @see Project
      @return true/false whether the path is a directory.
      */
    bool isDir(const QString &path);

    /**
      @brief Removes a file from the project.

      If the given path points to a directory, or if the file does not exist, this method will fail
      and return false
      @param path The path to delete. @see Project
      */
    bool removeFile(const QString &path);

    /**
      @brief Removes a directory from the project.

      If the given file points to a file, or if the directory does not exist, or if the directory is
      not empty, this method will fail and return false.
      @see removeAllDir to remove a non-empty directory.
      @param path The path to delete. @see Project
      */
    bool removeDir(const QString &path);

    /**
      @brief Removes a directory and all its content from the project.

      If the given file points to a file, or if the directory does not exist, this method will fail
      and return false.
      @warning This will remove every file stored in the specified directory.
      @param path The path to delete. @see Project
      */
    bool removeAllDir(const QString &path);

    /**
      @brief Copies a file from the local filesystem into the project

      @param source The absolute path of the file to copy
      @param destination The project path where to copy the file
      @return false if copy failed
      */
    bool copy(const QString &source, const QString &destination);

    /**
      @brief Creates an empty file

      This method is similar to the UNIX's touch shell command. Except it won't renew the file's
      creation date if the file already exists.
      @param file the file to create
      @return true if file creation was successful.
      */
    bool touch(const QString &path);

    /**
      @brief Gets a STL file stream (default to R/W access mode) to the given path.

      @warning This method does not check if the given path is a directory or a file. User might get
      an invalid filestream.
      @warning It is up to the user to delete the std::fstream returned.
      @param path The path to open. @see Project
      @return an opened filestream on the given path.
      */
    std::fstream *stdFileStream(const QString &path, std::ios_base::openmode = std::fstream::in |
                                                                               std::fstream::out |
                                                                               std::fstream::app);

    /**
      @brief Gets a Qt I/O device (default to R/W access mode) to the given path.

      @warning This method does not check if the given path is a directory or a file. User might get
      an invalid filestream.
      @warning User SHOULD NOT cast the QIODevice returned by this method into any of its subclass
      since the implementation might change in future versions.
      @warning It is up to the user to delete the QIODevice returned.
      @param path The path to open. @see Project
      @param mode The opening mode as described in the Qt documentation.
      @return an opened Qt device on the given path.
      */
    QIODevice *fileStream(const QString &path, QIODevice::OpenMode mode = QIODevice::ReadWrite);

    /**
      @brief Returns the archive file associated with this project.

      If the project has been opened from an existing file or if the write method has already been
      called, this method will return the last file path specified.
      In other cases, this method will return an empty string.
      */
    QString projectFile() const {
        return _projectFile;
    }

    /**
      @brief This method returns the real absolute path corresponding to / in the Project.

      This can be used to create a Project directly from a path.
      @warning Using several Project instances at the same time on the same path may result in
      undefined behavior.
      */
    QString absoluteRootPath() const;

    // Developer note: Every field in the Project tagged as a Q_PROPERTY will automatically be
    // serialized in the project.xml file
    /**
      @brief the name of the project
      */
    Q_PROPERTY(QString name READ name WRITE setName)
    /**
     * @see name
     */
    QString name() const;

    /**
      @brief User-written description of the project
      */
    Q_PROPERTY(QString description READ description WRITE setDescription)
    /**
     * @see description
     */
    QString description() const;

    /**
      @brief Name of the author
      */
    Q_PROPERTY(QString author READ author WRITE setAuthor)
    /**
     * @see author
     */
    QString author() const;

    /**
      @brief The version of the Talipot project format with which the file was created.
      Project from older format version will be always saved into the newest version available.
      */
    QString version() const;

    /**
      @brief Returns the absolute filesystem path used to store the file
      @warning Be cautious though since directly modifying project files without using Project
      methods could result in undefined behavior.
      */
    QString toAbsolutePath(const QString &relativePath);

  signals:
    void projectFileChanged(const QString &projectFile);

  public slots:
    /**
     * @see name
     */
    void setName(const QString &);
    /**
     * @see description
     */
    void setDescription(const QString &);
    /**
     * @see author
     */
    void setAuthor(const QString &);

  private:
    bool writeMetaInfo();
    bool readMetaInfo();

    // Core fileset
    QTemporaryDir *_rootDir;
    QString _projectFile;

    const QString rootDir() const {
        return _rootDir->path();
    }

    // Meta information
    QString _author;
    QString _name;
    QString _description;
};
}
#endif // TALIPOT_PROJECT_H
