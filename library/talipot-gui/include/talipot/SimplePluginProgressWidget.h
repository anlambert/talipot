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

#ifndef TALIPOT_SIMPLE_PLUGIN_PROGRESS_WIDGET_H
#define TALIPOT_SIMPLE_PLUGIN_PROGRESS_WIDGET_H

#include <talipot/PluginProgress.h>

#include <QTime>
#include <QWidget>
#include <QDialog>

namespace Ui {
class SimplePluginProgressWidget;
}

class QCloseEvent;

namespace tlp {

class TLP_QT_SCOPE SimplePluginProgressWidget : public QWidget, public tlp::PluginProgress {
    Q_OBJECT
    Ui::SimplePluginProgressWidget *_ui;

    QTime _lastUpdate;
    void checkLastUpdate();

    std::string _error;
    tlp::ProgressState _state;

  public:
    explicit SimplePluginProgressWidget(QWidget *parent = nullptr,
                                        Qt::WindowFlags f = Qt::WindowFlags());
    ~SimplePluginProgressWidget() override;

    void setComment(const std::string &) override;
    void setComment(const QString &);
    void setComment(const char *);

    void setTitle(const std::string &) override {}

    tlp::ProgressState progress(int step, int max_step) override;

    void cancel() override;
    void stop() override;

    bool isPreviewMode() const override;
    void setPreviewMode(bool drawPreview) override;
    void showPreview(bool showPreview) override;
    void showStops(bool showButtons) override;

    tlp::ProgressState state() const override;

    std::string getError() override;
    void setError(const std::string &error) override;

  protected:
    void closeEvent(QCloseEvent *) override;

  public slots:
    void setCancelButtonVisible(bool);
    void setStopButtonVisible(bool);

  protected slots:
    void cancelClicked();
    void stopClicked();
};

class TLP_QT_SCOPE SimplePluginProgressDialog : public QDialog, public tlp::PluginProgress {
    Q_OBJECT
  public:
    explicit SimplePluginProgressDialog(QWidget *parent = nullptr);
    ~SimplePluginProgressDialog() override;

    void setComment(const std::string &) override;
    void setComment(const QString &);
    void setComment(const char *);

    void setTitle(const std::string &title) override;

    tlp::ProgressState progress(int step, int max_step) override;

    void cancel() override;
    void stop() override;

    bool isPreviewMode() const override;
    void setPreviewMode(bool drawPreview) override;
    void showPreview(bool showPreview) override;
    void showStops(bool showButtons) override;

    tlp::ProgressState state() const override;

    std::string getError() override;
    void setError(const std::string &error) override;

  protected:
    void closeEvent(QCloseEvent *) override;
    void paintEvent(QPaintEvent *) override;
    bool _painted;

  public slots:
    void setCancelButtonVisible(bool v);
    void setStopButtonVisible(bool v);

  private:
    tlp::SimplePluginProgressWidget *_progress;
};
}

#endif // TALIPOT_SIMPLE_PLUGIN_PROGRESS_WIDGET_H
