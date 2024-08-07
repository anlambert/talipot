/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

namespace tlp {

enum class ProgressState {TLP_CONTINUE, TLP_CANCEL , TLP_STOP };

class PluginProgress /Abstract/ {
%Docstring
This interface allows to notify and control the progression of a process.
%End

%TypeHeaderCode
#include <talipot/PluginProgress.h>
%End

public:

%ConvertToSubClassCode
if (dynamic_cast<tlp::SimplePluginProgress *>(sipCpp)) {
  sipType = sipFindType("tlp::SimplePluginProgress");
} else {
  sipType = NULL;
}
%End

  PluginProgress();

  virtual tlp::ProgressState progress(int step, int max_step) = 0;
%Docstring
tlp.PluginProgress.progress(step, maxStep)

Use this method to notify the progression of the process.

:param step:
   the current step number

:type step:
   integer

:param maxStep:
   the total number of steps

:type maxStep:
   integer

:rtype:
   a value indicating whether the progress has been stopped
   (:const:`tlp.ProgressState.ProgressState::TLP_STOP`), cancelled
   (:const:`tlp.ProgressState.ProgressState::TLP_CANCEL`), or will continue
   (:const:`tlp.ProgressState.ProgressState::TLP_CONTINUE`).
%End


  virtual void cancel() = 0;
%Docstring
tlp.PluginProgress.cancel()

Sets the state flag to cancel, notifying to the process that the user wants to cancel it. Canceling
a process must stop it and revert all the changes performed since its start.
%End

  virtual void stop() = 0;
%Docstring
tlp.PluginProgress.stop()

Sets the state flag to stop, notifying to the process that the user wants to stop it. Stopping a
process does not revert changes.
%End

  virtual bool isPreviewMode() const = 0;
%Docstring
tlp.PluginProgress.isPreviewMode()

Returns :const:`True` if the preview mode is enabled. The preview mode redraws the graph while
applying the algorithm, making it slower.

:rtype:
   boolean
%End

  virtual void setPreviewMode(bool) = 0;
%Docstring
tlp.PluginProgress.setPreviewMode(preview)

Enables / disables the preview mode. The preview mode redraws the graph while applying the
algorithm, making it slower.

:param preview:
   indicates if the preview mode should be activated

:type preview:
   boolean
%End

  virtual void showPreview(bool) = 0;
%Docstring
tlp.PluginProgress.showPreview(showPreview)

This tells the widget associated to this PluginProgress if it should show a preview checkbox,
allowing the user to decide if the algorithm should draw a preview or not.

:param showPreview:
   whether the progress widget should contain a preview checkbox or not.

:type showPreview:
   boolean
%End

  virtual tlp::ProgressState state() const = 0;
%Docstring
tlp.PluginProgress.state()

Gets the current internal state of the PluginProgress.

:rtype:
   a value indicating whether the progress has been stopped
   (:const:`tlp.ProgressState.ProgressState::TLP_STOP`), cancelled
   (:const:`tlp.ProgressState.ProgressState::TLP_CANCEL`), or will continue
   (:const:`tlp.ProgressState.ProgressState::TLP_CONTINUE`).
%End

  virtual std::string getError() = 0;
%Docstring
tlp.PluginProgress.getError()

Returns a message describing the error encountered during the process. If no error has been
encountered, an empty string is returned.

:rtype:
   string
%End

  virtual void setError(std::string error) = 0;
%Docstring
tlp.PluginProgress.setError(errorMsg)

Sets the message describing the error encountered during the process.

:param errorMsg:
   the error message to set

:type errorMsg:
   string
%End

  virtual void setComment(std::string msg) = 0;
%Docstring
tlp.PluginProgress.setComment(msg)

Changes the comment about the process progression.

:param msg:
   A description of what the process is currently doing, displayed to inform the user.

:type msg:
   string
%End

};

class SimplePluginProgress : tlp::PluginProgress {
%TypeHeaderCode
#include <talipot/SimplePluginProgress.h>
%End

  public:

  SimplePluginProgress();

  virtual tlp::ProgressState progress(int step, int max_step);

  virtual void cancel();

  virtual void stop();

  virtual bool isPreviewMode() const;

  virtual void setPreviewMode(bool);

  virtual void showPreview(bool);

  virtual tlp::ProgressState state() const;

  virtual std::string getError();

  virtual void setError(const std::string& error);

  virtual void setComment(const std::string&);

  virtual void setTitle(const std::string&);

};
};
