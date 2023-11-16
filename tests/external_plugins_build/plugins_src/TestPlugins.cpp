#include <talipot/PluginHeaders.h>

// Check base plugins implementation can be compiled
// and successfully loaded

// Algorithm plugin
class TestAlgorithmPlugin : public tlp::Algorithm {

public:
  PLUGININFORMATION("Test Algorithm Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestAlgorithmPlugin(tlp::PluginContext *context) : tlp::Algorithm(context) {}

  bool run() override {
    graph->clear();
    return true;
  }
};

PLUGIN(TestAlgorithmPlugin)

// BooleanAlgorithm plugin
class TestBooleanAlgorithmPlugin : public tlp::BooleanAlgorithm {

public:
  PLUGININFORMATION("Test Boolean Algorithm Plugin", "The Talipot developers", "2019", "", "1.0",
                    "")

  TestBooleanAlgorithmPlugin(tlp::PluginContext *context) : tlp::BooleanAlgorithm(context) {}

  bool run() override {
    result->setAllNodeValue(false);
    result->setNodeValue(graph->getRandomNode(), true);
    return true;
  }
};

PLUGIN(TestBooleanAlgorithmPlugin)

// ColorAlgorithm plugin
class TestColorAlgorithmPlugin : public tlp::ColorAlgorithm {

public:
  PLUGININFORMATION("Test Color Algorithm Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestColorAlgorithmPlugin(tlp::PluginContext *context) : tlp::ColorAlgorithm(context) {}

  bool run() override {
    result->setAllNodeValue(tlp::Color::Red);
    result->setNodeValue(graph->getRandomNode(), tlp::Color::Green);
    return true;
  }
};

PLUGIN(TestColorAlgorithmPlugin)

// DoubleAlgorithm plugin
class TestDoubleAlgorithmPlugin : public tlp::DoubleAlgorithm {

public:
  PLUGININFORMATION("Test Double Algorithm Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestDoubleAlgorithmPlugin(tlp::PluginContext *context) : tlp::DoubleAlgorithm(context) {}

  bool run() override {
    result->setAllNodeValue(tlp::randomNumber());
    result->setNodeValue(graph->getRandomNode(), tlp::randomNumber());
    return true;
  }
};

PLUGIN(TestDoubleAlgorithmPlugin)

// IntegerAlgorithm plugin
class TestIntegerAlgorithmPlugin : public tlp::IntegerAlgorithm {

public:
  PLUGININFORMATION("Test Integer Algorithm Plugin", "The Talipot developers", "2019", "", "1.0",
                    "")

  TestIntegerAlgorithmPlugin(tlp::PluginContext *context) : tlp::IntegerAlgorithm(context) {}

  bool run() override {
    result->setAllNodeValue(tlp::randomNumber(10000));
    result->setNodeValue(graph->getRandomNode(), tlp::randomNumber(10000));
    return true;
  }
};

PLUGIN(TestIntegerAlgorithmPlugin)

// LayoutAlgorithm plugin
class TestLayoutAlgorithmPlugin : public tlp::LayoutAlgorithm {

public:
  PLUGININFORMATION("Test Layout Algorithm Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestLayoutAlgorithmPlugin(tlp::PluginContext *context) : tlp::LayoutAlgorithm(context) {}

  bool run() override {
    result->setAllNodeValue(tlp::Coord(0.0f, 0.0f));
    result->setNodeValue(graph->getRandomNode(), tlp::Coord(10.0f, 10.0f));
    return true;
  }
};

PLUGIN(TestLayoutAlgorithmPlugin)

// SizeAlgorithm plugin
class TestSizeAlgorithmPlugin : public tlp::SizeAlgorithm {

public:
  PLUGININFORMATION("Test Size Algorithm Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestSizeAlgorithmPlugin(tlp::PluginContext *context) : tlp::SizeAlgorithm(context) {}

  bool run() override {
    result->setAllNodeValue(tlp::Size(1.0f, 1.0f, 1.0f));
    result->setNodeValue(graph->getRandomNode(), tlp::Size(10.0f, 10.0f, 10.0f));
    return true;
  }
};

PLUGIN(TestSizeAlgorithmPlugin)

// StringAlgorithm plugin
class TestStringAlgorithmPlugin : public tlp::StringAlgorithm {

public:
  PLUGININFORMATION("Test String Algorithm Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestStringAlgorithmPlugin(tlp::PluginContext *context) : tlp::StringAlgorithm(context) {}

  bool run() override {
    result->setAllNodeValue("foo");
    result->setNodeValue(graph->getRandomNode(), "bar");
    return true;
  }
};

PLUGIN(TestStringAlgorithmPlugin)

// ImportModule plugin
class TestImportModulePlugin : public tlp::ImportModule {

public:
  PLUGININFORMATION("Test Import Module Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestImportModulePlugin(tlp::PluginContext *context) : tlp::ImportModule(context) {}

  bool importGraph() override {
    graph->addNode();
    return true;
  }
};

PLUGIN(TestImportModulePlugin)

// ExportModule plugin
class TestExportModulePlugin : public tlp::ExportModule {

public:
  PLUGININFORMATION("Test Export Module Plugin", "The Talipot developers", "2019", "", "1.0", "")

  TestExportModulePlugin(tlp::PluginContext *context) : tlp::ExportModule(context) {}

  bool exportGraph(std::ostream &os) override {
    os << graph;
    return true;
  }

  std::string fileExtension() const override {
    return "graph";
  }
};

PLUGIN(TestExportModulePlugin)

// Check there is no symbol issue due to Talipot multithreading
// implementation (either OpenMP or C++11 threads)
// when using a static property in a plugin code
class TestVectorPropertyPlugin : public tlp::Algorithm {

public:
  PLUGININFORMATION("Test Static Property", "The Talipot developers", "2019", "", "1.0", "")

  TestVectorPropertyPlugin(tlp::PluginContext *context) : tlp::Algorithm(context) {}

  bool run() override {
    tlp::NodeVectorProperty<double *> test(graph);
    test.setAll(nullptr);
    return true;
  }
};

PLUGIN(TestVectorPropertyPlugin)
