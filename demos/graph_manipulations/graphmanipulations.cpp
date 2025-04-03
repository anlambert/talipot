#include <talipot/PluginLoaderTxt.h>
#include <talipot/PluginLibraryLoader.h>
#include <talipot/ColorProperty.h>
#include <talipot/StringProperty.h>
#include <talipot/DoubleProperty.h>
#include <talipot/PluginsManager.h>

using namespace std;
using namespace tlp;

int main(int, char **) {

    /*
     * Let's create the following graph
     *
     *      A
     *    /   \
     *  B       C
     *   \     /
     *    D - E
     */

    /*
     Initialize the library and load all plugins
     */
    tlp::initTalipotLib();
    PluginLoaderTxt loadertxt;
    PluginLibraryLoader::loadPlugins(&loadertxt);

    // create a new graph
    Graph *myGraph = tlp::newGraph();

    node a = myGraph->addNode();
    node b = myGraph->addNode();
    node c = myGraph->addNode();
    node d = myGraph->addNode();
    node e = myGraph->addNode();

    myGraph->addEdge(a, b);
    myGraph->addEdge(a, c);
    myGraph->addEdge(b, d);
    myGraph->addEdge(c, e);
    myGraph->addEdge(d, e);

    // now in color. 'viewColor' is the Talipot GUI's default color property, so when we load it we
    // will see the color immediately If 'viewColor' did not exist before, this creates it.
    ColorProperty *color = myGraph->getColorProperty("viewColor");
    color->setNodeValue(a, Color(255, 0, 0));
    color->setNodeValue(b, Color(0, 255, 0));
    color->setNodeValue(c, Color(0, 0, 255));
    color->setNodeValue(d, Color(255, 0, 0));
    color->setNodeValue(e, Color(0, 255, 0));
    // hey look, this is a 3-coloration :)

    // set the label of the nodes (again, with Talipot's default label property)
    StringProperty *label = myGraph->getStringProperty("viewLabel");
    label->setNodeValue(a, "A");
    label->setNodeValue(b, "B");
    label->setNodeValue(c, "C");
    label->setNodeValue(d, "D");
    label->setNodeValue(e, "E");

    DoubleProperty *metric = myGraph->getDoubleProperty("degree");

    // if the degree plugin is available, let's call it.
    if (tlp::PluginsManager::pluginExists("Degree")) {
        // now compute the degree of the nodes.
        string errorMessage;
        // this calls the Talipot plugin 'Degree'.
        bool success = myGraph->applyPropertyAlgorithm("Degree", metric, errorMessage);

        if (!success) {
            std::cout << errorMessage << std::endl;
        }
    } else {
        std::cout << "could not find the plugin, computing" << std::endl;
        for (auto n : myGraph->nodes()) {
            metric->setNodeValue(n, myGraph->deg(n));
        }
    }

    // output the degree of node a;
    std::cout << metric->getNodeValue(a) << std::endl;

    // saveGraph is a shortcut ofr exportGraph that uses the TLP export.
    tlp::saveGraph(myGraph, "mygraph.tlp");

    return EXIT_SUCCESS;
}
