/**
 *
 * Copyright (C) 2019  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_PERSPECTIVE_H
#define TALIPOT_PERSPECTIVE_H

#include <talipot/PluginContext.h>
#include <talipot/Plugin.h>
#include <talipot/TlpQtTools.h>

#include <QString>
#include <QVariantMap>
#include <QSet>
#include <QMenu>

class QMainWindow;
class QTcpSocket;

namespace tlp {

class PluginProgress;
class Project;

static const std::string PERSPECTIVE_CATEGORY = "Perspective";

/**
 * @ingroup Plugins
 * @brief A context data structure for tlp::Perspective instances.
 *
 * @see tlp::Perspective
 */
class TLP_QT_SCOPE PerspectiveContext : public tlp::PluginContext {
public:
  PerspectiveContext() : mainWindow(nullptr), project(nullptr), talipotPort(0) {}
  QMainWindow *mainWindow;
  Project *project;
  QString externalFile;
  QVariantMap parameters;
  quint64 talipotPort;
  unsigned int id;
};

/**
 * @ingroup Plugins
 * @brief A Perspective is a Tulip plugin that completely re-defines the user interface.
 *
 * A Perspective aims at using the multiple features available in Tulip to create a complete,
 * coherent workflow dedicated to a particular use-case.
 * Perspectives are chosen by the user when first running the Tulip agent. The main perspective is
 * called "Tulip". This perspective unveils all the Tulip features and aims at being a protyping and
 * researching platform.
 *
 * A Perspective always acts in its own process and communicate with the Tulip agent via TCP
 * sockets.
 * Thus, it is the Perspective's responsibility to offer the possibility to display graphs, run
 * plugins, etc. A lot of helper classes can be found into the talipot-gui API like pre-made
 * widgets, Qt models, etc.
 *
 * Perspective's data is stored into a Project. A Project is an archive capable of
 * containing heterogeneous data (like multiple graphs, textures, extra files, etc). When a
 * Project gets saved by a perspective, it is linked to it in its meta-information.
 * Creating a Perspective primarily into implementing the start() method that builds the GUI. This
 * method will be called by the overleying system when a new perspective is launched. The rest of
 * the logic implementation is left to the developer.
 */
class TLP_QT_SCOPE Perspective : public QObject, public tlp::Plugin {
  Q_OBJECT

  static tlp::Perspective *_instance;
  QSet<QString> _reservedProperties;
  QTcpSocket *_agentSocket;
  unsigned int _perspectiveId;
  bool _maximised;
  void sendAgentMessage(const QString &);
  void notifyProjectLocation(const QString &path);

protected:
  /**
   * @brief The project associated to this perspective. This project can be empty or contain data
   * depending on how the Perspective was launched:
   * To launch a perspective, the talipot_perspective executable is called with the following
   * arguments:
   * @code
   * talipot_perspective [--perspective=Name] [file_path]
   * @endcode
   * @list
   * @li The --perspective argument forces Tulip to start the "Name" perspective. Even if the
   * project states otherwise. If this argument is not specified, Tulip will look in the project's
   * meta-data to dertermine the perspective to launch.
   * @li file_path is the absolute path of the project archive to associate with the perspective. If
   * file_path is not given, the --perspective argument must be declared in order for Tulip to know
   * what perspective to launch. In this case, _project will point to an empty (but valid!) project.
   * @endlist
   */
  Project *_project;

  /**
   * @brief The main window on which the perspective should build the GUI.
   *
   * It is not the Perspective's responsibility to destroy the main window when the application gets
   * closed. Generally speaking, destroying the _mainWindow pointer could lead to undefined
   * behavior.
   */
  QMainWindow *_mainWindow;

  /**
   * If the user provided a file to the talipot_perspective but this file is not recognized as a
   * valid Project, the full path of the file will be stored into the _externalFile member.
   * @note Remember that perspectives should always store their data into Project in order to
   * keep a consistent workflow.
   */
  QString _externalFile;

  /**
   * @brief Contains extra parameters that have not been parsed by the overleying system. Those are
   * considered to be Perspective-specific and are forwarded into this variable.
   */
  QVariantMap _parameters;
  bool checkSocketConnected();

public:
  enum ProgressOption {
    NoProgressOption = 0x0,
    IsPreviewable = 0x1,
    IsCancellable = 0x2,
    IsStoppable = 0x4
  };
  Q_DECLARE_FLAGS(ProgressOptions, ProgressOption)

  std::string category() const override {
    return PERSPECTIVE_CATEGORY;
  }
  std::string icon() const override {
    return ":/talipot/gui/icons/32/plugin_controller.png";
  }

  /**
   * @brief Called at the beginning of the talipot_perspective to set the Perspective singleton.
   * @see instance()
   */
  static void setInstance(tlp::Perspective *);

  /**
   * Since a Perspective has its own process to work in, there can be only one perspective instance
   * by process.
   * In order to ease the development process, Perspective are available as a singleton in order for
   * child widgets and plugins to be able to access to the features of a Perspective.
   * This architecture was added for developers who want to create an application containing several
   * plugins grouped into Perspective.
   * @return The perspective singleton
   */
  static tlp::Perspective *instance();

  /**
   * @return A typed instance of the perspective singleton.
   */
  template <typename T>
  static T *typedInstance() {
    return dynamic_cast<T *>(instance());
  }

  /**
   * @brief Constructs a perspective object
   * @warning There should not be any logic implemented into the Perspective's constructor. See the
   * start method instead.
   */
  Perspective(const tlp::PluginContext *c);
  ~Perspective() override;

  /**
   * @brief Build the main window GUI and starts the workflow.
   * When this method is called, it means that the Perspective is responsible of the application
   * workflow until the application is closed by the user.
   * @param progress A progress handler
   */
  virtual void start(tlp::PluginProgress *progress) = 0;

  /**
   * @brief Creates a progress handler and returns it.
   * This method allows lower-level widgets to create top-level progress bars directly from the
   * Perspective.
   * @return
   */
  virtual PluginProgress *
  progress(ProgressOptions options = ProgressOptions(IsPreviewable | IsStoppable | IsCancellable));

  /**
   * @return The Perspective's main window.
   */
  QMainWindow *mainWindow() const;

  /**
   * @brief Checks if the name corresponds to a reserved properties.
   *  Perspectives are allowed to reserve graph properties. A reserved graph properties is a core
   * property that cannot be deleted by the user and cannot be renamed.
   * @return true if the perspective is registered.
   */
  bool isReservedPropertyName(QString name);

  /**
   * @brief Sets a new property name as registered
   */
  void registerReservedProperty(QString);

  /**
   * @brief Tells the perspective that the graph visualizations should be redrawn.
   * @param center if true, visualization should also be centered (eg. the layout has been changed)
   */
  virtual void redrawPanels(bool center = false) = 0;

  /**
   * @brief Tells the perspective that the visualizations for a given graph should be centered.
   * @note By default, this method does nothing.
   */
  virtual void centerPanelsForGraph(tlp::Graph *);

  void resetTitle() {
    emit resetWindowTitle();
  }

public slots:
  /**
   * @brief Called when the user wants to close the application.
   * @return Returning false prevents the window from being closed but the Perspective will have to
   * implement its own way of closing the application.
   */
  virtual bool terminated() {
    return true;
  }

signals:
  void resetWindowTitle();

protected slots:
  /**
   * @brief Send a message to the Tulip agent to make him display the Plugins Center page.
   */
  void showPluginsCenter();

  /**
   * @brief Call this slot to switch to full screen or windowed mode
   * @param f is true, switch to full screen mode. If false, switch to windowed mode
   */
  void showFullScreen(bool f);

  /**
   * @brief Send a message to the Tulip agent to make him display the Projects page.
   */
  void showProjectsPage();

  /**
   * @brief Send a message to the Tulip agent to make him display the "About us" page.
   */
  void showAboutPage();

  /**
   * @brief Send a message to the Tulip agent to make him display a message in the system
   * notification area.
   * @param s The message to display.
   */
  void showTrayMessage(const QString &s);

  /**
   * @brief Send a message to the Tulip agent to make him display an error message that will be
   * shown in the system notification as well as on the welcome page.
   * @param title The message's title.
   * @param s The message to display.
   */
  void showErrorMessage(const QString &title, const QString &s);

  /**
   * @brief Send a message to the Tulip agent to make him open a new Tulip Project.
   * @param path the absolute path of the project file.
   */
  virtual void openProjectFile(const QString &path);

  /**
   * @brief Send a message to the Tulip agent to make him open a new Perspective without a project.
   * @param name The name of the PErspective to create.
   */
  void createPerspective(const QString &name);

public:
  /**
   * @brief usage Displays a usage message when called from the talipot_perspective executable
   */
  virtual void usage(std::string &usage_str) const {
    usage_str = "No options for this perspective.";
  }
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Perspective::ProgressOptions)
}

#endif // TALIPOT_PERSPECTIVE_H