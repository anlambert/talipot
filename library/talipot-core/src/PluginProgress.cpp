/**
 *
 * Copyright (C) 2019-2022  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/PluginProgress.h>

using namespace tlp;

PluginProgress::PluginProgress() : _previewHandler(nullptr) {}

PluginProgress::~PluginProgress() {
  delete _previewHandler;
}

void PluginProgress::setPreviewHandler(ProgressPreviewHandler *handler) {
  delete _previewHandler;
  _previewHandler = handler;
}
ProgressState PluginProgress::progress(int step, int max_step) {
  if (_previewHandler != nullptr && isPreviewMode()) {
    _previewHandler->progressStateChanged(step, max_step);
  }

  return ProgressState::TLP_CONTINUE;
}

ProgressPreviewHandler::~ProgressPreviewHandler() = default;
