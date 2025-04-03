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

#include <ogdf/layered/LongestPathRanking.h>
#include <ogdf/layered/OptimalRanking.h>
#include <ogdf/layered/BarycenterHeuristic.h>
#include <ogdf/layered/MedianHeuristic.h>
#include <ogdf/layered/SplitHeuristic.h>
#include <ogdf/layered/FastHierarchyLayout.h>
#include <ogdf/layered/CoffmanGrahamRanking.h>
#include <ogdf/layered/SiftingHeuristic.h>
#include <ogdf/layered/GreedySwitchHeuristic.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>
#include <ogdf/layered/FastSimpleHierarchyLayout.h>
#include <ogdf/layered/GridSifting.h>
#include <ogdf/layered/OptimalHierarchyLayout.h>

#include <talipot/StringCollection.h>
#include <talipot/OGDFLayoutPluginBase.h>

using namespace std;

#define ELT_RANKING "Ranking"
#define ELT_RANKINGLIST "LongestPathRanking;OptimalRanking;CoffmanGrahamRanking"
static const vector<function<ogdf::RankingModule *()>> ranking = {
    []() { return new ogdf::LongestPathRanking; },
    []() { return new ogdf::OptimalRanking; },
    []() { return new ogdf::CoffmanGrahamRanking; },
};

#define ELT_TWOLAYERCROSS "Two-layer crossing minimization"
#define ELT_TWOLAYERCROSSLIST                                              \
    "BarycenterHeuristic;MedianHeuristic;SplitHeuristic;SiftingHeuristic;" \
    "GreedyInsertHeuristic;GreedySwitchHeuristic;GlobalSiftingHeuristic;"  \
    "GridSiftingHeuristic"
static const vector<function<ogdf::LayeredCrossMinModule *()>> crossingMinimization = {
    []() { return new ogdf::BarycenterHeuristic; },   []() { return new ogdf::MedianHeuristic; },
    []() { return new ogdf::SplitHeuristic; },        []() { return new ogdf::SiftingHeuristic; },
    []() { return new ogdf::GreedySwitchHeuristic; }, []() { return new ogdf::GlobalSifting; },
    []() { return new ogdf::GridSifting; },
};

template <class T>
function<ogdf::HierarchyLayoutModule *(double, double, bool)> hierarchyLayoutFunc() {
    return [](double nodeDistance, double layerDistance, bool fixedLayerDistance) {
        T *layout = new T;
        layout->nodeDistance(nodeDistance);
        layout->layerDistance(layerDistance);
        if constexpr (tlp::type_name<T>() != tlp::type_name<ogdf::FastSimpleHierarchyLayout>()) {
            layout->fixedLayerDistance(nodeDistance);
        }
        return layout;
    };
};

#define ELT_HIERARCHYLAYOUT "Layout"
#define ELT_HIERARCHYLAYOUTLIST \
    "FastHierarchyLayout;FastSimpleHierarchyLayout;OptimalHierarchyLayout"
static const vector<function<ogdf::HierarchyLayoutModule *(double, double, bool)>> hLayout = {
    hierarchyLayoutFunc<ogdf::FastHierarchyLayout>(),
    hierarchyLayoutFunc<ogdf::FastSimpleHierarchyLayout>(),
    hierarchyLayoutFunc<ogdf::OptimalHierarchyLayout>(),
};

static constexpr std::string_view paramHelp[] = {
    // fails
    "The number of times that the number of crossings may not decrease after a complete top-down "
    "bottom-up traversal, before a run is terminated.",

    // runs
    "Determines, how many times the crossing minimization is repeated. Each repetition (except for "
    "the first) starts with randomly permuted nodes on each layer. Deterministic behaviour can be "
    "achieved by setting runs to 1.",

    // node distance
    "The minimal horizontal distance between two nodes on the same layer.",

    // layer distance
    "The minimal vertical distance between two nodes on neighboring layers.",

    // fixed layer distance
    "If true, the distance between neighboring layers is fixed, otherwise variable (not "
    "available for FastSimpleHierarchyLayout).",

    // transpose
    "If this option is set to true an additional fine tuning step is performed after each "
    "traversal, which tries to reduce the total number of crossings by switching adjacent vertices "
    "on the same layer.",

    // arrangeCCs
    "If set to true connected components are laid out separately and the resulting layouts are "
    "arranged afterwards using the packer module.",

    // minDistCC
    "Specifies the spacing between connected components of the graph.",

    // pageRatio
    "The page ratio used for packing connected components.",

    // alignBaseClasses
    "Determines if base classes of inheritance hierarchies shall be aligned.",

    // alignSiblings
    "Sets the option alignSiblings.",

    // Ranking
    "Sets the option for the node ranking (layer assignment).",

    // Two-layer crossing minimization
    "Sets the module option for the two-layer crossing minimization.",

    // Layout
    "The hierarchy layout module that computes the final layout.",

    // transpose vertically
    "Transpose the layout vertically from top to bottom."};

static const char *eltRankingValuesDescription =
    "<b>CoffmanGrahamRanking</b> <i>(The coffman graham ranking algorithm)</i><br>"
    "<b>LongestPathRanking</b> <i>(the well-known longest-path ranking algorithm)</i><br>"
    "<b>OptimalRanking</b> <i>(the LP-based algorithm for computing a node ranking with minimal "
    "edge lengths)</i>";

static const char *twoLayerCrossValuesDescription =
    "<b>BarycenterHeuristic</b> <i>(the barycenter heuristic for 2-layer crossing "
    "minimization)</i><br>"
    "<b>GreedyInsertHeuristic</b> <i>(The greedy-insert heuristic for 2-layer crossing "
    "minimization)</i><br>"
    "<b>GreedySwitchHeuristic</b> <i>(The greedy-switch heuristic for 2-layer crossing "
    "minimization)</i><br>"
    "<b>MedianHeuristic</b> <i>(the median heuristic for 2-layer crossing minimization)</i><br>"
    "<b>SiftingHeuristic</b> <i>(The sifting heuristic for 2-layer crossing minimization)</i><br>"
    "<b>SplitHeuristic</b> <i>(the split heuristic for 2-layer crossing minimization)</i><br>"
    "<b>GridSiftingHeuristic</b> <i>(the grid sifting heuristic for 2-layer crossing "
    "minimization)</i><br>"
    "<b>GlobalSiftingHeuristic</b> <i>(the global sifting heuristic for 2-layer crossing "
    "minimization)</i>";

static const char *hierarchyLayoutValuesDescription =
    "<b>FastHierarchyLayout</b> <i>(Coordinate assignment phase for the Sugiyama algorithm by "
    "Buchheim et al.)</i><br>"
    "<b>FastSimpleHierarchyLayout</b> <i>(Coordinate assignment phase for the Sugiyama algorithm "
    "by Ulrik Brandes and Boris Koepf)</i><br>"
    "<b>OptimalHierarchyLayout</b> <i>(The LP-based hierarchy layout algorithm)</i>";

class OGDFSugiyama : public tlp::OGDFLayoutPluginBase {

  public:
    PLUGININFORMATION("Sugiyama (OGDF)", "Carsten Gutwenger", "12/11/2007",
                      "Implements the classical layout algorithm by Sugiyama, Tagawa, and Toda. It "
                      "is a layer-based approach for producing upward drawings.",
                      "1.7", "Hierarchical")

    OGDFSugiyama(const tlp::PluginContext *context)
        : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::SugiyamaLayout>(context)) {
        addInParameter<int>("fails", paramHelp[0].data(), "4");
        addInParameter<int>("runs", paramHelp[1].data(), "15");
        addInParameter<double>("node distance", paramHelp[2].data(), "3");
        addInParameter<double>("layer distance", paramHelp[3].data(), "3");
        addInParameter<bool>("fixed layer distance", paramHelp[4].data(), "false");
        addInParameter<bool>("transpose", paramHelp[5].data(), "true");
        addInParameter<bool>("arrangeCCs", paramHelp[6].data(), "true");
        addInParameter<double>("minDistCC", paramHelp[7].data(), "20");
        addInParameter<double>("pageRatio", paramHelp[8].data(), "1.0");
        addInParameter<bool>("alignBaseClasses", paramHelp[9].data(), "false");
        addInParameter<bool>("alignSiblings", paramHelp[10].data(), "false");
        addInParameter<tlp::StringCollection>(ELT_RANKING, paramHelp[11].data(), ELT_RANKINGLIST,
                                              true, eltRankingValuesDescription);
        addInParameter<tlp::StringCollection>(ELT_TWOLAYERCROSS, paramHelp[12].data(),
                                              ELT_TWOLAYERCROSSLIST, true,
                                              twoLayerCrossValuesDescription);
        addInParameter<tlp::StringCollection>(ELT_HIERARCHYLAYOUT, paramHelp[13].data(),
                                              ELT_HIERARCHYLAYOUTLIST, true,
                                              hierarchyLayoutValuesDescription);
        addInParameter<bool>("transpose vertically", paramHelp[14].data(), "true");
    }

    void beforeCall() override {

        auto *sugiyama = static_cast<ogdf::SugiyamaLayout *>(ogdfLayoutAlgo);

        if (dataSet != nullptr) {
            int ival = 0;
            double dval = 0;
            bool bval = false;
            tlp::StringCollection sc;

            if (dataSet->get("fails", ival)) {
                sugiyama->fails(ival);
            }

            if (dataSet->get("runs", ival)) {
                sugiyama->runs(ival);
            }

            if (dataSet->get("arrangeCCS", bval)) {
                sugiyama->arrangeCCs(bval);
            }

            if (dataSet->get("minDistCC", dval)) {
                sugiyama->minDistCC(dval);
            }

            if (dataSet->get("pageRatio", dval)) {
                sugiyama->pageRatio(dval);
            }

            if (dataSet->get("alignBaseClasses", bval)) {
                sugiyama->alignBaseClasses(bval);
            }

            if (dataSet->get("alignSiblings", bval)) {
                sugiyama->alignSiblings(bval);
            }

            if (dataSet->get("transpose", bval)) {
                sugiyama->transpose(bval);
            }

            if (dataSet->get(ELT_RANKING, sc)) {
                sugiyama->setRanking(ranking[sc.getCurrent()]());
            }

            if (dataSet->get(ELT_TWOLAYERCROSS, sc)) {
                sugiyama->setCrossMin(crossingMinimization[sc.getCurrent()]());
            }

            if (dataSet->get(ELT_HIERARCHYLAYOUT, sc)) {
                double nodeDistance = 3;
                double layerDistance = 3;
                bool fixedLayerDistance = true;
                dataSet->get("node distance", nodeDistance);
                dataSet->get("layer distance", layerDistance);
                dataSet->get("fixed layer distance", fixedLayerDistance);

                sugiyama->setLayout(
                    hLayout[sc.getCurrent()](nodeDistance, layerDistance, fixedLayerDistance));
            }
        }
    }

    void callOGDFLayoutAlgorithm(ogdf::GraphAttributes &gAttributes) override {
        auto *sugiyama = static_cast<ogdf::SugiyamaLayout *>(ogdfLayoutAlgo);

        if (sugiyama->alignBaseClasses() || sugiyama->alignSiblings()) {
            sugiyama->callUML(gAttributes);
        } else {
            ogdfLayoutAlgo->call(gAttributes);
        }
    }

    void afterCall() override {
        if (dataSet != nullptr) {
            bool bval = false;

            if (dataSet->get("transpose vertically", bval)) {
                if (bval) {
                    transposeLayoutVertically();
                }
            }
        }
    }
};

PLUGIN(OGDFSugiyama)
