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

#ifndef TALIPOT_ORDERING_H
#define TALIPOT_ORDERING_H

#include <talipot/Face.h>
#include <talipot/Node.h>
#include <talipot/Edge.h>

#include <talipot/MutableContainer.h>

namespace tlp {
class PluginProgress;
class PlanarConMap;

class TLP_SCOPE Ordering {
  public:
    typedef struct FaceAndPos_ {
        Face face;
        node n_first;
        node n_last;
    } FaceAndPos;

    std::vector<edge> getDummyEdges() {
        return dummy_edge;
    }

    Ordering(PlanarConMap *G, PluginProgress *pluginProgress = nullptr, int minProgress = 0,
             int deltaProgress = 0, int maxProgress = 0);
    ~Ordering();
    //    void push_back(std::vector<node> nodeVector) {
    size_t size() {
        return _data.size();
    }
    std::vector<node> operator[](const uint i) const {
        return _data[i];
    }
    std::vector<node> &operator[](const uint i) {
        return _data[i];
    }

  private:
    std::vector<std::vector<node>> _data;
    PlanarConMap *Gp;
    MutableContainer<int> oute;
    MutableContainer<int> outv;
    MutableContainer<bool> visitedNodes;
    MutableContainer<bool> visitedFaces;
    MutableContainer<bool> markedFaces;
    MutableContainer<int> seqP;
    MutableContainer<bool> isOuterFace;
    MutableContainer<bool> contour;
    MutableContainer<bool> is_selectable;
    MutableContainer<bool> is_selectable_visited;
    MutableContainer<bool> is_selectable_face;
    MutableContainer<bool> is_selectable_visited_face;
    MutableContainer<node> left;
    MutableContainer<node> right;
    bool existMarkedF;
    FaceAndPos minMarkedFace;
    Face ext;
    std::vector<node> v1;
    std::vector<edge> dummy_edge;

    node getLastOfQ(Face f, node prec, node n, edge e);
    node getLastOfP(Face f, node prec, node n, edge e);
    std::vector<node> getPathFrom(std::vector<node> fn, int from);
    int infFaceSize();

    void updateOutAndVisitedFaces(Face f);
    void updateContourLeftRight(node prec, node n, edge e, node last);
    void updateNewSelectableNodes(node node_f, node no_tmp2, edge ed_tmp, node node_last,
                                  std::vector<Face> v_faces, bool one_face = false,
                                  bool was_visited = false, bool selection_face = false);
    void updateSelectableFaces(std::vector<Face> v_faces);

    int seqp(Face f);
    void minMarkedf();
    void setMinMarkedFace(Face f);

    struct augmentableAndNodes_ getAugAndNodes(Face f);
    void augment(Face f, node prec, node n, node prec_last, node last, int nbNewFace, bool pair);
    void selectAndUpdate(Face f);
    void selectAndUpdate(node n);
    bool isSelectable(node n);

    void init();
    void init_v1(std::vector<node> fn);
    void init_selectableNodes();
    void init_selectableFaces();
    void init_outv_oute();
    void init_seqP();
    void init_outerface();
};
}
#endif // TALIPOT_ORDERING_H
