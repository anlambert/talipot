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

#ifndef TALIPOT_SCENE_LAYERS_MODEL_H
#define TALIPOT_SCENE_LAYERS_MODEL_H

#include <talipot/config.h>
#include <talipot/Observable.h>
#include <talipot/Model.h>

/**
  @brief Oh, you know, just a model for a GlScene entities
  */
namespace tlp {
class GlScene;

class TLP_QT_SCOPE SceneLayersModel : public Model, tlp::Observable {
    Q_OBJECT

    tlp::GlScene *_scene;

    QModelIndex glGraphIndex() const;

  public:
    explicit SceneLayersModel(tlp::GlScene *scene, QObject *parent = nullptr);

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void treatEvent(const tlp::Event &) override;

  signals:
    void drawNeeded(tlp::GlScene *);
};
}

#endif // TALIPOT_SCENE_LAYERS_MODEL_H
