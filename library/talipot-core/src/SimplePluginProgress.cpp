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

#include <talipot/SimplePluginProgress.h>
using namespace tlp;
using namespace std;
SimplePluginProgress::SimplePluginProgress()
    : _state(ProgressState::TLP_CONTINUE), _preview(false) {}
ProgressState SimplePluginProgress::progress(int step, int max_step) {
  PluginProgress::progress(step, max_step);
  progress_handler(step, max_step);
  return _state;
}
//====================================================
void SimplePluginProgress::stop() {
  _state = ProgressState::TLP_STOP;
}
//====================================================
void SimplePluginProgress::cancel() {
  _state = ProgressState::TLP_CANCEL;
}
//====================================================
bool SimplePluginProgress::isPreviewMode() const {
  return _preview;
}
//====================================================
void SimplePluginProgress::setPreviewMode(bool b) {
  if (b == _preview) {
    return;
  }

  _preview = b;
  preview_handler(_preview);
}
//====================================================
void SimplePluginProgress::showPreview(bool) {}
//====================================================
void SimplePluginProgress::showStops(bool) {}
//====================================================
ProgressState SimplePluginProgress::state() const {
  return _state;
}
//====================================================
void SimplePluginProgress::progress_handler(int, int) {}
//====================================================
void SimplePluginProgress::preview_handler(bool) {}
//====================================================
