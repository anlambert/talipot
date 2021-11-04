/**
 *
 * Copyright (C) 2019-2021  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#ifndef TALIPOT_GL_META_NODE_RENDERER_H
#define TALIPOT_GL_META_NODE_RENDERER_H

#include <talipot/config.h>
#include <talipot/Observable.h>
#include <unordered_map>

namespace tlp {

class GlGraphInputData;
class Camera;
class Graph;
class GlScene;

/**
 * Class used to render a meta node
 */
class TLP_GL_SCOPE GlMetaNodeRenderer : public Observable {

public:
  GlMetaNodeRenderer(GlGraphInputData *inputData);

  ~GlMetaNodeRenderer() override;

  virtual void render(node, float, Camera *);

  virtual void setInputData(GlGraphInputData *inputData);

  virtual GlGraphInputData *inputData() const;

  GlScene *getSceneForMetaGraph(Graph *g) const;

protected:
  void clearScenes();

  void treatEvent(const Event &) override;

  virtual GlScene *createScene(Graph *) const;

private:
  GlGraphInputData *_inputData;
  std::unordered_map<Graph *, GlScene *> _metaGraphToSceneMap;
};
}

#endif // TALIPOT_GL_META_NODE_RENDERER_H
