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

#include "DatasetTools.h"

#define ORTHOGONAL "orthogonal"
#define ORIENTATION "up to down;down to up;right to left;left to right;"
#define ORIENTATION_ID "orientation"

using namespace tlp;

static constexpr std::string_view paramHelp[] = {
    // orientation
    "Choose a desired orientation.",

    // orthogonal
    "If true then use orthogonal edges.",

    // layer spacing
    "This parameter enables to set up the minimum space between two layers in the drawing.",

    // node spacing
    "This parameter enables to set up the minimum space between two nodes in the same layer.",

    // node size
    "This parameter defines the property used for node sizes."};

//====================================================================
void addOrientationParameters(LayoutAlgorithm *pLayout) {
    pLayout->addInParameter<StringCollection>(ORIENTATION_ID, paramHelp[0].data(), ORIENTATION,
                                              true,
                                              "<b>up to down</b> <br> <b>down to up</b> <br> "
                                              "<b>right to left</b> <br> <b>left to right</b>");
}

//====================================================================
void addOrthogonalParameters(LayoutAlgorithm *pLayout) {
    pLayout->addInParameter<bool>(ORTHOGONAL, paramHelp[1].data(), "false");
}

//====================================================================
void addSpacingParameters(LayoutAlgorithm *pLayout) {
    pLayout->addInParameter<float>("layer spacing", paramHelp[2].data(), "64.");
    pLayout->addInParameter<float>("node spacing", paramHelp[3].data(), "18.");
}
void getSpacingParameters(DataSet *dataSet, float &nodeSpacing, float &layerSpacing) {
    layerSpacing = 64.;
    nodeSpacing = 18.;

    if (dataSet) {
        dataSet->get("node spacing", nodeSpacing);
        dataSet->get("layer spacing", layerSpacing);
    }
}
//====================================================================
void addNodeSizePropertyParameter(LayoutAlgorithm *pLayout, bool inout) {
    if (inout) {
        pLayout->addInOutParameter<SizeProperty>("node size", paramHelp[4].data(), "viewSize");
    } else {
        pLayout->addInParameter<SizeProperty>("node size", paramHelp[4].data(), "viewSize");
    }
}

bool getNodeSizePropertyParameter(DataSet *dataSet, SizeProperty *&sizes) {
    return (dataSet != nullptr) && dataSet->get("node size", sizes) && sizes;
}

//====================================================================
DataSet setOrientationParameters(int pOrientation) {
    DataSet dataSet;
    StringCollection stringOrientation(ORIENTATION);
    stringOrientation.setCurrent(pOrientation);
    dataSet.set(ORIENTATION_ID, stringOrientation);
    return dataSet;
}

//====================================================================
orientationType getMask(DataSet *dataSet) {
    StringCollection orientation(ORIENTATION);
    orientation.setCurrent(0);
    int current = 0;

    if (dataSet != nullptr) {
        StringCollection dataSetOrientation;

        if (dataSet->get(ORIENTATION_ID, dataSetOrientation)) {
            // the order of ORIENTATION items may have change
            // because the default value may have change (see DataSetDialog.cpp)
            std::string currentString = dataSetOrientation.getCurrentString();

            for (current = 0; current < 4; ++current) {
                if (currentString == orientation.at(current)) {
                    break;
                }
            }
        }
    }

    switch (current) {
    case 0:
        return ORI_DEFAULT;

    case 1:
        return ORI_INVERSION_VERTICAL;

    case 2:
        return ORI_ROTATION_XY;

    case 3:
        return orientationType(ORI_ROTATION_XY | ORI_INVERSION_HORIZONTAL);

    default:
        return ORI_DEFAULT;
    }
}

//====================================================================
bool hasOrthogonalEdge(DataSet *dataSet) {
    bool orthogonalEdge = false;

    if (dataSet != nullptr) {
        dataSet->get(ORTHOGONAL, orthogonalEdge);
    }

    return orthogonalEdge;
}
