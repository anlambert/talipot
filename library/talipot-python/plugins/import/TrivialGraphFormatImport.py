# Copyright (C) 2023  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

# That plugin imports a graph from a file in the Trivial Graph Format (TGF)
# (see https://en.wikipedia.org/wiki/Trivial_Graph_Format)

import talipotplugins

from talipot import tlp


class TrivialGraphFormatImport(tlp.ImportModule):
    def __init__(self, context):
        super().__init__(context)
        self.addFileParameter("filename", True, "The TGF file to import")

    def fileExtensions(self):
        return ["tgf"]

    def parseTGF(self, tgfFile):
        parsingEdges = False
        nodesMap = {}
        for line in tgfFile:
            line = line.rstrip()

            if not line:
                continue

            if line.startswith("#"):
                parsingEdges = True
                continue

            if not parsingEdges:
                split = line.split(maxsplit=1)
                n = self.graph.addNode()
                nodesMap[split[0]] = n
                self.graph["viewLabel"][n] = split[1] if len(split) > 1 else split[0]
            else:
                split = line.split(maxsplit=2)
                if len(split) < 2:
                    raise ValueError(f"Invalid edge definition: {line}")
                for i in range(2):
                    if split[i] not in nodesMap:
                        raise ValueError(
                            f"Invalid node identifier found when parsing edges: {split[i]}"
                        )
                e = self.graph.addEdge(nodesMap[split[0]], nodesMap[split[1]])
                if len(split) > 2:
                    self.graph["viewLabel"][e] = split[2]

    def importGraph(self):
        # parse the TGF file
        with open(self.dataSet["filename"], "r") as tgfFile:
            self.parseTGF(tgfFile)

        return True


pluginDoc = """
<p>Supported extension: tgf</p><p>Imports a graph from a file in the
Trivial Graph Format (https://en.wikipedia.org/wiki/Trivial_Graph_Format).
The format consists of a list of node definitions, which map node IDs to labels,
followed by a list of edges, which specify node pairs and an optional edge label.</p>
"""

# The line below does the magic to register the plugin to the plugin database
# and updates the GUI to make it accessible through the menus.
talipotplugins.registerPluginOfGroup(
    "TrivialGraphFormatImport",
    "Trivial Graph Format (TGF)",
    "Antoine Lambert",
    "16/11/2023",
    pluginDoc,
    "1.0",
    "File",
)
