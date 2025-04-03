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

#include <talipot/FaceIterator.h>
#include <talipot/PlanarConMap.h>

using namespace std;
using namespace tlp;

//============================================================
// Iterator for Face : FaceIterator
//============================================================
FaceIterator::FaceIterator(PlanarConMap *m) : i(0), mgraph(m) {
    assert(m);
}

//============================================================
/**
 * return the next element
 */
Face FaceIterator::next() {
    return mgraph->faces[i++];
}

//============================================================
/**
 * test if there's a next element
 */
bool FaceIterator::hasNext() {
    return (i != mgraph->faces.size());
}

//============================================================
// Iterator for Face : FaceAdjIterator
//============================================================
FaceAdjIterator::FaceAdjIterator(PlanarConMap *m, const node n) : i(0) {
    assert(m->isElement(n));
    facesAdj.erase(facesAdj.begin(), facesAdj.end());
    edge e;
    Face f_tmp, f_tmp2;
    Iterator<edge> *ite = m->getInOutEdges(n);

    while (ite->hasNext()) {
        e = ite->next();

        if (m->edgesFaces.find(e) != m->edgesFaces.end()) {
            f_tmp = (m->edgesFaces[e])[0];
            f_tmp2 = (m->edgesFaces[e])[1];
            break;
        }
    }

    if (ite->hasNext()) {
        while (ite->hasNext()) {
            e = ite->next();

            if (m->edgesFaces.find(e) != m->edgesFaces.end()) {
                if (f_tmp == (m->edgesFaces[e])[0]) {
                    facesAdj.push_back(f_tmp);
                    f_tmp = (m->edgesFaces[e])[1];
                    facesAdj.push_back(f_tmp);
                } else if (f_tmp == (m->edgesFaces[e])[1]) {
                    facesAdj.push_back(f_tmp);
                    f_tmp = (m->edgesFaces[e])[0];
                    facesAdj.push_back(f_tmp);
                } else if (f_tmp2 == (m->edgesFaces[e])[0]) {
                    facesAdj.push_back(f_tmp2);
                    f_tmp = (m->edgesFaces[e])[1];
                    facesAdj.push_back(f_tmp);
                } else if (f_tmp2 == (m->edgesFaces[e])[1]) {
                    facesAdj.push_back(f_tmp2);
                    f_tmp = (m->edgesFaces[e])[0];
                    facesAdj.push_back(f_tmp);
                }

                break;
            }
        }

    } else {
        facesAdj.push_back(f_tmp);
        //  facesAdj.push_back(f_tmp2);
    }

    while (ite->hasNext()) {
        e = ite->next();

        if (m->edgesFaces.find(e) != m->edgesFaces.end()) {
            if (f_tmp == (m->edgesFaces[e])[0]) {
                f_tmp = (m->edgesFaces[e])[1];
                facesAdj.push_back((m->edgesFaces[e])[1]);
            } else {
                f_tmp = (m->edgesFaces[e])[0];
                facesAdj.push_back((m->edgesFaces[e])[0]);
            }
        }
    }

    delete ite;
}

//============================================================
/**
 * return the next element
 */
Face FaceAdjIterator::next() {
    return facesAdj[i++];
}

//============================================================
/**
 * test if there's a next element
 */
bool FaceAdjIterator::hasNext() {
    return (i != facesAdj.size());
}

//============================================================
// NodeFaceIterator
//============================================================
NodeFaceIterator::NodeFaceIterator(PlanarConMap *m, const Face face) : i(0) {
    const vector<edge> &e = m->facesEdges[face];
    edge e1 = e[0];
    edge e2 = e[1];
    node prev;
    const auto &[e1Src, e1Tgt] = m->ends(e1);
    const auto &[e2Src, e2Tgt] = m->ends(e2);

    if (e1Src == e2Src || e1Src == e2Tgt) {
        prev = e1Src;
    } else {
        prev = e1Tgt;
    }

    nodes.push_back(prev);

    for (uint j = 1; j < m->facesEdges[face].size(); ++j) {
        e1 = m->facesEdges[face][j];
        const auto &[e1Src, e1Tgt] = m->ends(e1);

        if (e1Src == prev) {
            prev = e1Tgt;
            nodes.push_back(prev);
        } else {
            prev = e1Src;
            nodes.push_back(prev);
        }
    }
}

//============================================================
/**
 * return the next element
 */
node NodeFaceIterator::next() {
    return nodes[i++];
}

//============================================================
/**
 * test if there's a next element
 */
bool NodeFaceIterator::hasNext() {
    return (i != nodes.size());
}

//============================================================
// EdgeFaceIterator
//============================================================
EdgeFaceIterator::EdgeFaceIterator(PlanarConMap *m, const Face f) : ve(m->facesEdges[f]), i(0) {}

//============================================================
/**
 * return the next element
 */
edge EdgeFaceIterator::next() {
    return ve[i++];
}

//============================================================
/**
 * test if there's a next element
 */
bool EdgeFaceIterator::hasNext() {
    return (i != ve.size());
}
