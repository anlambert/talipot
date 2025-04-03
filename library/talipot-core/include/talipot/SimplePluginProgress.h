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

#ifndef TALIPOT_SIMPLE_PLUGIN_PROGRESS_H
#define TALIPOT_SIMPLE_PLUGIN_PROGRESS_H

#include <talipot/PluginProgress.h>
#include <string>
namespace tlp {
/**
 * @brief Basic implementation for PluginProgress interface.
 *
 **/
class TLP_SCOPE SimplePluginProgress : public PluginProgress {
  public:
    SimplePluginProgress();
    ProgressState progress(int step, int max_step) override;
    void cancel() override;
    void stop() override;
    bool isPreviewMode() const override;
    void setPreviewMode(bool) override;
    void showPreview(bool) override;
    void showStops(bool) override;
    ProgressState state() const override;
    std::string getError() override {
        return _error;
    }
    void setError(const std::string &error) override {
        _error = error;
    }
    void setComment(const std::string &) override {}
    void setTitle(const std::string &) override {}

  protected:
    virtual void progress_handler(int step, int max_step);
    // call when the _preview variable change.
    virtual void preview_handler(bool);

  private:
    ProgressState _state;
    bool _preview;
    std::string _error;
};
}
#endif // TALIPOT_SIMPLE_PLUGIN_PROGRESS_H
