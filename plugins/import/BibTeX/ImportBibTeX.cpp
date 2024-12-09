/**
 *
 * Copyright (C) 2019-2024  The Talipot developers
 *
 * Talipot is a fork of Tulip, created by David Auber
 * and the Tulip development Team from LaBRI, University of Bordeaux
 *
 * See the AUTHORS file at the top-level directory of this distribution
 * License: GNU General Public License version 3, or any later version
 * See top-level LICENSE file for more information
 *
 */

#include <talipot/PluginHeaders.h>
#include <talipot/ViewSettings.h>
#include <talipot/FontAwesome.h>

#include <xdkbibtex/file.h>
#include <xdkbibtex/author.h>

using namespace std;
using namespace tlp;

#define IMPORT_AUTHORS 0
#define IMPORT_ALL 1
#define IMPORT_PUBLICATIONS 2

#define NODES_TO_IMPORT "Authors;Authors & Publications;Publications"

extern string &forceUtf8String(string &);
extern string &normalizeString(string &);

class ImportBibTeX : public ImportModule {

public:
  PLUGININFORMATION("BibTeX", "Patrick Mary", "09/01/2014",
                    "<p>Supported extensions: bib</p><p>Import a co-authorship graph from a BibTeX "
                    "formatted file.</p>",
                    "1.1", "File")

  ImportBibTeX(const PluginContext *context) : ImportModule(context) {
    addInParameter<string>(
        "file::filename", "This parameter indicates the pathname of the file(.bib) to import.", "");
    addInParameter<StringCollection>(
        "Nodes to import",
        "The type of nodes to create: <b>Authors</b> (Create nodes for authors only, publications "
        "are represented as edges between authors)<br/><b>Authors and Publications</b> (Create "
        "nodes for both authors and publications and edges are created between the publications "
        "and their authors)<br/><b>Publications</b> (Create nodes for publications only)",
        NODES_TO_IMPORT);
    addInParameter<bool>(
        "One edge per publication",
        "When only <b>Authors</b> are imported, this parameter indicates:<ul><li>if set to "
        "<b>true</b>, that a new edge will be created each time two authors are involved in the "
        "same publication.</li><li>if set to <b>false</b>, that only one edge will be created "
        "between two authors involved in at least one publication.<br/>Then the <b># "
        "publications</b> property edge value will indicate the number of publications they wrote "
        "in common.</li></ul>",
        "true", false);
  }

  string icon() const override {
    return ":/talipot/app/icons/32/import_bibtex.png";
  }

  list<string> fileExtensions() const override {
    return {"bib"};
  }

  bool importGraph() override {
    string filename;
    int toImport = IMPORT_AUTHORS;
    bool oneEdgePerPubli = true;

    if (dataSet) {
      dataSet->get<string>("file::filename", filename);
      StringCollection nodesToImport(NODES_TO_IMPORT);
      nodesToImport.setCurrent(toImport);

      if (dataSet->get("Nodes to import", nodesToImport)) {
        toImport = nodesToImport.getCurrent();
      }
      dataSet->get("One edge per publication", oneEdgePerPubli);
    }

    if (filename.empty()) {
      pluginProgress->setError("Filename is empty.");
      return false;
    }

    bool result = false;
    bool createAuthNodes = toImport != IMPORT_PUBLICATIONS;
    bool createPubliNodes = toImport != IMPORT_AUTHORS;

    // known properties to extract
    StringProperty *keyProp = oneEdgePerPubli ? graph->getStringProperty("key") : nullptr;
    StringProperty *typeProp = oneEdgePerPubli ? graph->getStringProperty("type") : nullptr;
    IntegerProperty *yearProp = oneEdgePerPubli ? graph->getIntegerProperty("year") : nullptr;
    StringVectorProperty *authProp =
        createPubliNodes ? graph->getStringVectorProperty("authors") : nullptr;
    StringProperty *authNameProp = createAuthNodes ? graph->getStringProperty("name") : nullptr;
    IntegerProperty *countProp =
        createAuthNodes ? graph->getIntegerProperty("# publications") : nullptr;

    // rendering properties
    ColorProperty *color = graph->getColorProperty("viewColor");
    StringProperty *icon = graph->getStringProperty("viewIcon");
    StringProperty *label = graph->getStringProperty("viewLabel");

    graph->getIntegerProperty("viewLabelPosition")->setAllNodeValue(LabelPosition::Bottom);
    graph->getIntegerProperty("viewShape")->setAllNodeValue(NodeShape::Icon);

    flat_hash_map<string, node> authorsMap;
    flat_hash_map<string, bool> publisMap;

    try {
      xdkbib::File bibFile;
      // extract entries from BibTeX file
      bibFile.readFromFile(filename, xdkbib::File::StrictQuote);
      auto &entries = const_cast<vector<xdkbib::FileEntry> &>(bibFile.entries());

      // first add nodes for publication
      vector<node> publis;

      if (createPubliNodes) {
        publis = graph->addNodes(entries.size());
      }

      // loop on entries
      uint i = 0;

      for (auto &fe : entries) {

        node publi;

        if (createPubliNodes) {
          publi = publis[i];
        }

        // set extracted properties
        string key = fe.key();

        // first check if publi does not already exist
        if (publisMap.contains(key)) {
          --i;
          continue;
        }

        publisMap[key] = true;
        int year = atoi(fe.field("year").value().c_str());

        if (createPubliNodes) {
          // setup key, type and year
          keyProp->setNodeValue(publi, key);
          string type;
          transform(fe.type().begin(), fe.type().end(), type.begin(), ::tolower);
          typeProp->setNodeValue(publi, type);
          yearProp->setNodeValue(publi, year);
          // set rendering properties
          color->setNodeValue(publi, Color::Beige);
          label->setNodeValue(publi, key);
          icon->setNodeValue(publi, FontAwesome::Regular::FileAlt);
        }

        // loop of entry fields
        for (const auto &fit : fe.fields()) {
          string pName = fit.first;

          // year is already set
          if (pName == "year") {
            continue;
          }

          const xdkbib::Field &field = fit.second;
          bool isNumber =
              !field.valueParts().empty() && (field.valueParts()[0].type() == xdkbib::Number);
          bool isAuthor = (pName == "author");
          PropertyInterface *prop = nullptr;

          if (createPubliNodes) {
            if (isNumber && (pName != "volume") && (pName != "number")) {
              // create an IntegerProperty
              prop = graph->getIntegerProperty(pName);
            } else {
              if (isAuthor) {
                prop = authProp;
              } else {
                // create a StringProperty
                prop = graph->getStringProperty(pName);
              }
            }
          }

          string value = fe.field(pName).value();

          if (createPubliNodes) {
            if (!isAuthor) {
              if (!isNumber) {
                forceUtf8String(value);
              }

              prop->setNodeStringValue(publi, value);
            }
          }

          if (isAuthor) {
            // add authors
            xdkbib::AuthorList authors;
            authors.readFrom(fe.field("author").value().c_str());
            vector<string> authPropValue;
            vector<node> authorNodes;

            for (uint j = 0; j < authors.size(); ++j) {

              xdkbib::Author &auth = authors[j];
              vector<string> firstNames = auth.first();
              string aName;
              string aKey;

              for (uint k = 0; k < firstNames.size(); ++k) {
                string &firstName = firstNames[k];

                if (k) {
                  aName += " ";
                  aKey += " ";
                }

                aName += firstName;

                if (k == 0) {
                  // keep only the first letter and a dot for the key
                  firstName.resize(2);
                  firstName.replace(1, 1, ".");
                }

                aKey += firstName;
              }

              aName += " " + auth.lastJoin();
              forceUtf8String(aName);
              // author must be a unique person
              authPropValue.push_back(aName);

              if ((aName == "al. et") || !createAuthNodes) {
                continue;
              }

              aKey += " " + auth.lastJoin();
              forceUtf8String(aKey);

              normalizeString(aKey);

              node author;
              // check if the author already exists
              auto itAuth = authorsMap.find(aKey);

              if (itAuth != authorsMap.end()) {
                authorNodes.push_back(author = itAuth->second);
                countProp->setNodeValue(author, countProp->getNodeValue(author) + 1);
              } else {
                authorNodes.push_back(author = graph->addNode());
                authorsMap[aKey] = author;
                authNameProp->setNodeValue(author, aName);
                label->setNodeValue(author, aName);
                icon->setNodeValue(author, FontAwesome::Solid::User);
                countProp->setNodeValue(author, 1);
              }

              if (createPubliNodes) {
                // add edge from author to publi
                edge e = graph->addEdge(author, publi);
                // set year
                yearProp->setEdgeValue(e, year);
              }
            }

            if (createPubliNodes) {
              // store authors
              authProp->setNodeValue(publi, authPropValue);
            } else {
              // display a warning for publication without author
              if (authorNodes.empty()) {
                warning() << "Warning: found the publication '" << key.c_str() << "' without author"
                          << endl;
                // stop processing publication fields
                break;
              }
              auto addLink = [&](node a1, node a2) {
                edge e = graph->existEdge(a1, a2);
                uint cnt = 0;
                if (e.isValid()) {
                  cnt = countProp->getEdgeValue(e);
                } else {
                  e = graph->addEdge(a1, a2);
                }
                countProp->setEdgeValue(e, cnt + 1);
              };
              // create edges between the authors of the publications
              if (authorNodes.size() == 1) {
                if (oneEdgePerPubli) {
                  // create a loop to record publication information
                  edge e = graph->addEdge(authorNodes[0], authorNodes[0]);
                  // setup key, type and year
                  keyProp->setEdgeValue(e, key);
                  typeProp->setEdgeValue(e, fe.type());
                  yearProp->setEdgeValue(e, year);
                } else {
                  addLink(authorNodes[0], authorNodes[0]);
                }
              } else {
                if (oneEdgePerPubli) {
                  for (uint j = 0; j < authorNodes.size() - 1; ++j) {
                    for (uint k = j + 1; k < authorNodes.size(); ++k) {
                      edge e = graph->addEdge(authorNodes[j], authorNodes[k]);
                      // setup key, type and year
                      keyProp->setEdgeValue(e, key);
                      typeProp->setEdgeValue(e, fe.type());
                      yearProp->setEdgeValue(e, year);
                    }
                  }
                } else {
                  for (uint j = 0; j < authorNodes.size() - 1; ++j) {
                    for (uint k = j + 1; k < authorNodes.size(); ++k) {
                      addLink(authorNodes[j], authorNodes[k]);
                    }
                  }
                }
              }
            }
          }
        }
        ++i;
      }

      // in case of duplicate
      // delete unused publi nodes
      if (createPubliNodes) {
        for (; i < entries.size(); ++i) {
          graph->delNode(publis[i]);
        }
      }
    } catch (xdkbib::parsing_error &e) {
      stringstream sstr;
      sstr << "BibTeX file parsing error at char " << e.column() << " of line " << e.line() << ": "
           << e.what() << endl;
      pluginProgress->setError(sstr.str());
    }

    result = !graph->isEmpty();

    if (!result) {
      pluginProgress->setError("Import of graph from BibTeX file " + filename + " failed.");
    } else if (createAuthNodes) {
      // layout graph with FM^3
      string err;
      graph->applyPropertyAlgorithm("FM^3 (OGDF)", graph->getLayoutProperty("viewLayout"), err,
                                    nullptr, pluginProgress);
    }

    return result;
  }
};

PLUGIN(ImportBibTeX)
