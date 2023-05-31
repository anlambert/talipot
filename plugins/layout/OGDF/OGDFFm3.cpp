/**
 *
 * Copyright (C) 2019-2023  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/OGDFLayoutPluginBase.h>
#include <talipot/NumericProperty.h>
#include <talipot/StringCollection.h>
#include <talipot/SizeProperty.h>

#include <ogdf/energybased/FMMMLayout.h>

using namespace std;

#define ELT_PAGEFORMAT "Page Format"
#define ELT_PAGEFORMATLIST "Square;Portrait;Landscape"
static const vector<ogdf::FMMMOptions::PageFormatType> pageFormatType = {
    ogdf::FMMMOptions::PageFormatType::Square,
    ogdf::FMMMOptions::PageFormatType::Portrait,
    ogdf::FMMMOptions::PageFormatType::Landscape,
};

#define ELT_QUALITYVSSPEED "Quality vs Speed"
#define ELT_QUALITYVSSPEEDLIST "BeautifulAndFast;NiceAndIncredibleSpeed;GorgeousAndEfficient"
static const vector<ogdf::FMMMOptions::QualityVsSpeed> qualityVsSpeed = {
    ogdf::FMMMOptions::QualityVsSpeed::BeautifulAndFast,
    ogdf::FMMMOptions::QualityVsSpeed::NiceAndIncredibleSpeed,
    ogdf::FMMMOptions::QualityVsSpeed::GorgeousAndEfficient,
};

#define ELT_EDGELENGTHMEASUREMENT "Edge Length Measurement"
#define ELT_EDGELENGTHMEASUREMENTLIST "BoundingCircle;Midpoint"
static const vector<ogdf::FMMMOptions::EdgeLengthMeasurement> edgeLengthMeasurement = {
    ogdf::FMMMOptions::EdgeLengthMeasurement::BoundingCircle,
    ogdf::FMMMOptions::EdgeLengthMeasurement::Midpoint,
};

#define ELT_ALLOWEDPOSITIONS "Allowed Positions"
#define ELT_ALLOWEDPOSITIONSLIST "Integer;Exponent;All"
static const vector<ogdf::FMMMOptions::AllowedPositions> allowedPositions = {
    ogdf::FMMMOptions::AllowedPositions::Integer,
    ogdf::FMMMOptions::AllowedPositions::Exponent,
    ogdf::FMMMOptions::AllowedPositions::All,
};

#define ELT_TIPOVER "Tip Over"
#define ELT_TIPOVERLIST "NoGrowingRow;Always;None"
static const vector<ogdf::FMMMOptions::TipOver> tipOver = {
    ogdf::FMMMOptions::TipOver::NoGrowingRow,
    ogdf::FMMMOptions::TipOver::Always,
    ogdf::FMMMOptions::TipOver::None,
};

#define ELT_PRESORT "Pre Sort"
#define ELT_PRESORTLIST "DecreasingHeight;DecreasingWidth;None;"
static const vector<ogdf::FMMMOptions::PreSort> preSort = {
    ogdf::FMMMOptions::PreSort::DecreasingHeight,
    ogdf::FMMMOptions::PreSort::DecreasingWidth,
};

#define ELT_GALAXYCHOICE "Galaxy Choice"
#define ELT_GALAXYCHOICELIST "NonUniformProbLowerMass;NonUniformProbHigherMass;UniformProb"
static const vector<ogdf::FMMMOptions::GalaxyChoice> galaxyChoice = {
    ogdf::FMMMOptions::GalaxyChoice::NonUniformProbLowerMass,
    ogdf::FMMMOptions::GalaxyChoice::NonUniformProbHigherMass,
    ogdf::FMMMOptions::GalaxyChoice::UniformProb,
};

#define ELT_MAXITERCHANGE "Max Iter Change"
#define ELT_MAXITERCHANGELIST "LinearlyDecreasing;RapidlyDecreasing;Constant"
static const vector<ogdf::FMMMOptions::MaxIterChange> maxIterChange = {
    ogdf::FMMMOptions::MaxIterChange::LinearlyDecreasing,
    ogdf::FMMMOptions::MaxIterChange::RapidlyDecreasing,
    ogdf::FMMMOptions::MaxIterChange::Constant,
};

#define ELT_INITIALPLACEMENTMULT "Initial Placement Mult"
#define ELT_INITIALPLACEMENTMULTLIST "Advanced;Simple"
static const vector<ogdf::FMMMOptions::InitialPlacementMult> initialPlacementMult = {
    ogdf::FMMMOptions::InitialPlacementMult::Advanced,
    ogdf::FMMMOptions::InitialPlacementMult::Simple,
};

#define ELT_FORCEMODEL "Force Model"
#define ELT_FORCEMODELLIST "New;FruchtermanReingold;Eades"
static const vector<ogdf::FMMMOptions::ForceModel> forceModel = {
    ogdf::FMMMOptions::ForceModel::New,
    ogdf::FMMMOptions::ForceModel::FruchtermanReingold,
    ogdf::FMMMOptions::ForceModel::Eades,
};

#define ELT_REPULSIVEFORCEMETHOD "Repulsive Forces Method"
#define ELT_REPULSIVEFORCEMETHODLIST "NMM;Exact;GridApproximation"
static const vector<ogdf::FMMMOptions::RepulsiveForcesMethod> repulsiveForcesMethod = {
    ogdf::FMMMOptions::RepulsiveForcesMethod::NMM,
    ogdf::FMMMOptions::RepulsiveForcesMethod::Exact,
    ogdf::FMMMOptions::RepulsiveForcesMethod::GridApproximation,
};

#define ELT_REDUCEDTREECONSTRCUCTION "Reduced Tree Construction"
#define ELT_REDUCEDTREECONSTRCUCTIONLIST "SubtreeBySubtree;PathByPath"
static const vector<ogdf::FMMMOptions::ReducedTreeConstruction> reducedTreeConstruction = {
    ogdf::FMMMOptions::ReducedTreeConstruction::SubtreeBySubtree,
    ogdf::FMMMOptions::ReducedTreeConstruction::PathByPath,
};

#define ELT_SMALLESTCELLFINDING "Smallest Cell Finding"
#define ELT_SMALLESTCELLFINDINGLIST "Iteratively;Aluru"
static const vector<ogdf::FMMMOptions::SmallestCellFinding> smallestCellFinding = {
    ogdf::FMMMOptions::SmallestCellFinding::Iteratively,
    ogdf::FMMMOptions::SmallestCellFinding::Aluru,
};

static constexpr std::string_view paramHelp[] = {

    // Edge Length Property
    "A numeric property containing unit edge length to use.",

    // Node Size
    "The node sizes.",

    // Unit edge length
    "The unit edge length.",

    // New initial placement
    "Defines if the initial placement of the nodes at the coarsest multilevel vary for each "
    "distinct call of the layout or keeps always the same.",

    // Threshold
    "The threshold for the stop criterion.",

    // Page Format
    "Possible page formats.",

    // Quality vs Speed
    "Trade-off between run-time and quality.",

    // Edge Length Measurement
    "Specifies how the length of an edge is measured.",

    // Allowed Positions
    "Specifies which positions for a node are allowed.",

    // Tip Over
    "Specifies in which case it is allowed to tip over drawings of connected components.",

    // Pre Sort
    "Specifies how connected components are sorted before the packing algorithm is applied.",

    // Galaxy Choice
    "Specifies how sun nodes of galaxies are selected.",

    // Max Iter Change
    "Specifies how MaxIterations is changed in subsequent multilevels.",

    // Initial Placement
    "Specifies how the initial placement is generated.",

    // Force Model
    "Specifies the force-model.",

    // Repulsive Forces Model
    "Specifies how to calculate repulsive forces.",

    // Reduced Tree Construction
    "Specifies how the reduced bucket quadtree is constructed.",

    // Smallest Cell Finding
    "Specifies how to calculate the smallest quadratic cell surrounding particles of a node in "
    "the reduced bucket quadtree.",

    // MaxIntPosExponent
    "Defines the exponent used if allowedPositions == Exponent (clamped to [31, 51])"};

static const char *pageFormatValuesDescription = "<b>Portrait</b> <i>(A4 portrait page)</i><br>"
                                                 "<b>Landscape</b> <i>(A4 landscape page)</i><br>"
                                                 "<b>Square</b> <i>(Square format)</i>";

static const char *qualityVsSpeedValuesDescription =
    "<b>GorgeousAndEfficient</b> <i>(Best quality)</i><br>"
    "<b>BeautifulAndFast</b> <i>(Medium quality and speed)</i><br>"
    "<b>NiceAndIncredibleSpeed</b> <i>(Best speed</i>";

static const char *edgeLengthMeasurementValuesDescription =
    "<b>Midpoint</b> <i>(Measure from center point of edge end points)</i><br>"
    "<b>BoundingCircle</b> <i>(Measure from border of circle surrounding edge end points)</i>";

static const char *presortValuesDescription =
    "<b>None</b> <i>(Do not presort)</i><br>"
    "<b>DecreasingHeight</b> <i>(Presort by decreasing height of components)</i><br>"
    "<b>DecreasingWidth</b> <i>(Presort by decreasing width of components)</i>";

static const char *galaxyChoiceValuesDescription = "<b>UniformProb</b><br>"
                                                   "<b>NonUniformProbLowerMass</b><br>"
                                                   "<b>NonUniformProbHigherMass</b>";

static const char *maxIterChangeValuesDescription = "<b>Constant</b> <br>"
                                                    "<b>LinearlyDecreasing</b> <br>"
                                                    "<b>RapidlyDecreasing</b>";

static const char *forceModelValuesDescription =
    "<b>FruchtermanReingold</b> <i>(The force-model by Fruchterman, Reingold)</i><br>"
    "<b>Eades</b> <i>(The force-model by Eades)</i><br>"
    "<b>New</b> <i>(The new force-model)</i>";

static const char *repulsiveForceValuesDescription =
    "<b>Exact</b> <i>(Exact calculation)</i><br>"
    "<b>GridApproximation</b> <i>(Grid approximation)</i><br>"
    "<b>NMM</b> <i>(Calculation as for new multipole method)</i>";

static const char *smallestCellFindingValuesDescription =
    "<b>Iteratively</b> <i>(Iteratively, in constant time)</i><br>"
    "<b>Aluru</b> <i>(According to formula by Aluru et al., in constant time)</i>";

static const char *allowedPositionsValuesDescription =
    "<b>All</b> <i>(Every position is allowed)</i><br>"
    "<b>Integer</b> <i>(Only integer positions are allowed that are in<br>"
    "a range depending on the number of nodes and<br>"
    "the average ideal edge length)</i><br>"
    "<b>Exponent</b> <i>(Only integer positions in a range of<br>"
    "[-2^MaxIntPosExponent, 2^MaxIntPosExponent] are allowed)</i>";

class OGDFFm3 : public tlp::OGDFLayoutPluginBase {

  tlp::StringCollection stringCollection;

public:
  PLUGININFORMATION("FM^3 (OGDF)", "Stephan Hachul", "09/11/2007",
                    "Implements the FM³ layout algorithm by Hachul and Jünger. It is a multilevel, "
                    "force-directed layout algorithm that can be applied to very large graphs.",
                    "1.4", "Force Directed")
  OGDFFm3(const tlp::PluginContext *context);
  void beforeCall() override;
  void callOGDFLayoutAlgorithm(ogdf::GraphAttributes &gAttributes) override;
};

PLUGIN(OGDFFm3)

OGDFFm3::OGDFFm3(const tlp::PluginContext *context)
    : OGDFLayoutPluginBase(context, tlp::getOGDFLayoutModule<ogdf::FMMMLayout>(context)) {
  addInParameter<tlp::NumericProperty *>("Edge Length Property", paramHelp[0].data(), "viewMetric",
                                         false);
  addInParameter<tlp::SizeProperty>("Node Size", paramHelp[1].data(), "viewSize", false);
  addInParameter<double>("Unit edge length", paramHelp[2].data(), "10.0", false);
  addInParameter<bool>("New initial placement", paramHelp[3].data(), "true");
  addInParameter<double>("Threshold", paramHelp[4].data(), "0.01");
  addInParameter<tlp::StringCollection>(ELT_PAGEFORMAT, paramHelp[5].data(), ELT_PAGEFORMATLIST,
                                        true, pageFormatValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_QUALITYVSSPEED, paramHelp[6].data(),
                                        ELT_QUALITYVSSPEEDLIST, true,
                                        qualityVsSpeedValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_EDGELENGTHMEASUREMENT, paramHelp[7].data(),
                                        ELT_EDGELENGTHMEASUREMENTLIST, true,
                                        edgeLengthMeasurementValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_ALLOWEDPOSITIONS, paramHelp[8].data(),
                                        ELT_ALLOWEDPOSITIONSLIST, true,
                                        allowedPositionsValuesDescription);
  addInParameter<int>("MaxIntPosExponent", paramHelp[18].data(), "40");
  addInParameter<tlp::StringCollection>(ELT_TIPOVER, paramHelp[9].data(), ELT_TIPOVERLIST, true,
                                        "<b>None</b><br> <b>NoGrowingRow</b><br> <b>Always</b>");
  addInParameter<tlp::StringCollection>(ELT_PRESORT, paramHelp[10].data(), ELT_PRESORTLIST, true,
                                        presortValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_GALAXYCHOICE, paramHelp[11].data(),
                                        ELT_GALAXYCHOICELIST, true, galaxyChoiceValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_MAXITERCHANGE, paramHelp[12].data(),
                                        ELT_MAXITERCHANGELIST, true,
                                        maxIterChangeValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_INITIALPLACEMENTMULT, paramHelp[13].data(),
                                        ELT_INITIALPLACEMENTMULTLIST, true,
                                        "<b>Simple</b> <br> <b>Advanced</b>");
  addInParameter<tlp::StringCollection>(ELT_FORCEMODEL, paramHelp[14].data(), ELT_FORCEMODELLIST,
                                        true, forceModelValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_REPULSIVEFORCEMETHOD, paramHelp[15].data(),
                                        ELT_REPULSIVEFORCEMETHODLIST, true,
                                        repulsiveForceValuesDescription);
  addInParameter<tlp::StringCollection>(ELT_REDUCEDTREECONSTRCUCTION, paramHelp[16].data(),
                                        ELT_REDUCEDTREECONSTRCUCTIONLIST, true,
                                        "<b>PathByPath</b> <br> <b>SubtreeBySubtree</b>");
  addInParameter<tlp::StringCollection>(ELT_SMALLESTCELLFINDING, paramHelp[17].data(),
                                        ELT_SMALLESTCELLFINDINGLIST, true,
                                        smallestCellFindingValuesDescription);
}

void OGDFFm3::beforeCall() {
  auto *fmmm = static_cast<ogdf::FMMMLayout *>(ogdfLayoutAlgo);

  if (dataSet != nullptr) {
    fmmm->useHighLevelOptions(true);
    tlp::SizeProperty *size = nullptr;

    if (dataSet->get("Node Size", size)) {
      tlpToOGDF->copyTlpNodeSizeToOGDF(size);
    }

    double edgeLenth = 10;

    if (dataSet->get("Unit edge length", edgeLenth)) {
      fmmm->unitEdgeLength(edgeLenth);
    }

    bool bval = false;

    if (dataSet->get("New initial placement", bval)) {
      fmmm->newInitialPlacement(bval);
    }

    double dval = 0;

    if (dataSet->get("Threshold", dval)) {
      fmmm->threshold(dval);
    }

    if (dataSet->get(ELT_PAGEFORMAT, stringCollection)) {
      fmmm->pageFormat(pageFormatType[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_QUALITYVSSPEED, stringCollection)) {
      fmmm->qualityVersusSpeed(qualityVsSpeed[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_EDGELENGTHMEASUREMENT, stringCollection)) {
      fmmm->edgeLengthMeasurement(edgeLengthMeasurement[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_ALLOWEDPOSITIONS, stringCollection)) {
      fmmm->allowedPositions(allowedPositions[stringCollection.getCurrent()]);
    }

    int ival = 0;

    if (dataSet->get("MaxIntPosExponent", ival)) {
      fmmm->maxIntPosExponent(ival);
    }

    if (dataSet->get(ELT_TIPOVER, stringCollection)) {
      fmmm->tipOverCCs(tipOver[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_PRESORT, stringCollection)) {
      fmmm->presortCCs(preSort[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_GALAXYCHOICE, stringCollection)) {
      fmmm->galaxyChoice(galaxyChoice[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_MAXITERCHANGE, stringCollection)) {
      fmmm->maxIterChange(maxIterChange[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_INITIALPLACEMENTMULT, stringCollection)) {
      fmmm->initialPlacementMult(initialPlacementMult[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_FORCEMODEL, stringCollection)) {
      fmmm->forceModel(forceModel[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_REPULSIVEFORCEMETHOD, stringCollection)) {
      fmmm->repulsiveForcesCalculation(repulsiveForcesMethod[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_REDUCEDTREECONSTRCUCTION, stringCollection)) {
      fmmm->nmTreeConstruction(reducedTreeConstruction[stringCollection.getCurrent()]);
    }

    if (dataSet->get(ELT_SMALLESTCELLFINDING, stringCollection)) {
      fmmm->nmSmallCell(smallestCellFinding[stringCollection.getCurrent()]);
    }
  }
}

void OGDFFm3::callOGDFLayoutAlgorithm(ogdf::GraphAttributes &gAttributes) {

  auto *fmmm = static_cast<ogdf::FMMMLayout *>(ogdfLayoutAlgo);

  tlp::NumericProperty *length = nullptr;

  if (dataSet->get("Edge Length Property", length) && length) {
    ogdf::EdgeArray<double> edgeLength(tlpToOGDF->getOGDFGraph());

    for (auto e : graph->edges()) {
      edgeLength[tlpToOGDF->getOGDFGraphEdge(e)] = length->getEdgeDoubleValue(e);
    }

    fmmm->call(gAttributes, edgeLength);
  } else {
    OGDFLayoutPluginBase::callOGDFLayoutAlgorithm(gAttributes);
  }
}
