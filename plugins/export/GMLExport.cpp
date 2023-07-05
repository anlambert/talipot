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

#include <iomanip>

#include <talipot/PluginHeaders.h>

using namespace std;
using namespace tlp;

void printFloat(ostream &os, const string &name, float f) {
  /*float a=floor(f);
  float b=(f-a)*1000;
  if (b<0) b*=-1;*/
  os << name << f << endl;
}

void printCoord(ostream &os, const Coord &v) {
  printFloat(os, "x ", v.getX());
  printFloat(os, "y ", v.getY());
  printFloat(os, "z ", v.getZ());
}
void printPoint(ostream &os, const Coord &v) {
  os << "point [" << endl;
  printCoord(os, v);
  os << "]" << endl;
}
void printSize(ostream &os, const Size &v) {
  printFloat(os, "h ", v.getW());
  printFloat(os, "w ", v.getH());
  printFloat(os, "d ", v.getD());
}

/**
 * This plugin records a Talipot graph structure using the GML File format.
 * This format is the file format used by Graphlet.
 * See www.infosun.fmi.uni-passau.de/Graphlet/GML/ for details.
 */
class GMLExport : public tlp::ExportModule {
public:
  PLUGININFORMATION("GML Export", "Auber David", "31/07/2001",
                    "<p>Supported extensions: gml</p><p>Exports a graph into a file using the "
                    "GML format (used by Graphlet).<br/>See "
                    "https://github.com/GunterMueller/UNI_PASSAU_FMI_Graph_Drawing/blob/master/GML/"
                    "gml-technical-report.pdf for details.</p>",
                    "1.0", "File")
  std::string fileExtension() const override {
    return "gml";
  }

  GMLExport(tlp::PluginContext *context) : tlp::ExportModule(context) {}

  ~GMLExport() override = default;

  string convert(const string &tmp) {
    string newStr;

    for (char c : tmp) {
      if (c == '\"') {
        newStr += "\\\"";
      } else {
        newStr += c;
      }
    }

    return newStr;
  }

  bool exportGraph(ostream &os) override {

    os << "graph [" << endl;
    os << "directed 1" << endl;
    os << "version 2" << endl;

    LayoutProperty *layout = graph->getLayoutProperty("viewLayout");
    StringProperty *label = graph->getStringProperty("viewLabel");
    ColorProperty *colors = graph->getColorProperty("viewColor");
    SizeProperty *sizes = graph->getSizeProperty("viewSize");

    // Save Nodes
    for (auto n : graph->nodes()) {
      os << "node [" << endl;
      os << "id " << n.id << endl;
      os << "label \"" << convert(label->getNodeValue(n)) << "\"" << endl;
      os << "graphics [" << endl;
      printCoord(os, layout->getNodeValue(n));
      printSize(os, sizes->getNodeValue(n));
      os << "type \"rectangle\"" << endl;
      os << "width 0.12" << endl;
      os << "fill \"#" << hex << setfill('0') << setw(2) << int(colors->getNodeValue(n).getR())
         << hex << setfill('0') << setw(2) << int(colors->getNodeValue(n).getG()) << hex
         << setfill('0') << setw(2) << int(colors->getNodeValue(n).getB()) << "\"" << endl;

      //      os << "outline \"#"<< hex << setfill('0') << setw(2)
      //      <<(int)colors->getNodeValue(n).getR()
      //         << hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(n).getG()
      //         << hex << setfill('0') << setw(2) <<(int)colors->getNodeValue(n).getB() <<
      //         "\""<< endl;

      os << "outline \"#000000\"" << endl;
      os << dec << setfill(' ') << setw(6) << "]" << endl;
      os << ']' << endl;
    }

    // Save edges
    for (auto e : graph->edges()) {
      const auto &[src, tgt] = graph->ends(e);
      os << "edge [" << endl;
      os << "source " << src.id << endl;
      os << "target " << tgt.id << endl;
      os << "id " << e.id << endl;
      os << "label \"" << label->getEdgeValue(e) << "\"" << endl;
      os << "graphics [" << endl;
      os << "type \"line\"" << endl;
      os << "arrow \"last\"" << endl;
      os << "width 0.1" << endl;
      os << "Line [" << endl;

      vector<Coord> lcoord = layout->getEdgeValue(e);

      if (!lcoord.empty()) {
        printPoint(os, layout->getNodeValue(src));
      }

      for (const auto &c : lcoord) {
        printPoint(os, c);
      }

      if (!lcoord.empty()) {
        printPoint(os, layout->getNodeValue(tgt));
      }

      os << "]" << endl;
      os << "]" << endl;
      os << "]" << endl;
    }

    os << "]" << endl;
    return true;
  }
};

PLUGIN(GMLExport)
