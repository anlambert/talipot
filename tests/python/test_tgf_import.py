# Copyright (C) 2023  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

import os
import tempfile
import textwrap
import unittest

from talipot import tlp


class TestTGFImportPlugin(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tgfPluginName = "Trivial Graph Format (TGF)"
        if "TALIPOT_BUILD_DIR" in os.environ:
            talipot_build_dir = os.environ["TALIPOT_BUILD_DIR"]
            if talipot_build_dir:
                tlp.loadPluginsFromDir(
                    os.path.join(talipot_build_dir, "library/talipot-python")
                )

    def test_tgf_plugin_loaded(self):
        self.assertTrue("Trivial Graph Format (TGF)" in tlp.getImportPluginsList())

    def test_load_tgf_graph_no_labels(self):
        with tempfile.TemporaryDirectory() as tgfDir:
            tgfFile = os.path.join(tgfDir, "graph.tgf")
            with open(tgfFile, "w") as tgfFileWriter:
                tgfFileWriter.write(
                    textwrap.dedent(
                        """
                        0
                        1
                        2
                        3
                        #
                        0 1
                        0 2
                        1 2
                        1 3
                        3 0
                        """
                    )
                )
            graph = tlp.importGraph(self.tgfPluginName, {"filename": tgfFile})
            self.assertTrue(graph is not None)
            self.assertEqual(graph.numberOfNodes(), 4)
            self.assertEqual(graph.numberOfEdges(), 5)
            nodes = graph.nodes()
            self.assertTrue(graph.existEdge(nodes[0], nodes[1]))
            self.assertTrue(graph.existEdge(nodes[0], nodes[2]))
            self.assertTrue(graph.existEdge(nodes[1], nodes[2]))
            self.assertTrue(graph.existEdge(nodes[1], nodes[3]))
            self.assertTrue(graph.existEdge(nodes[3], nodes[0]))
            self.assertEqual(
                [graph["viewLabel"][n] for n in graph.nodes()], ["0", "1", "2", "3"]
            )

    def test_load_tgf_graph_with_labels(self):
        with tempfile.TemporaryDirectory() as tgfDir:
            tgfFile = os.path.join(tgfDir, "graph.tgf")
            with open(tgfFile, "w") as tgfFileWriter:
                tgfFileWriter.write(
                    textwrap.dedent(
                        """
                        0 Node 0
                        1 Node 1
                        2 Node 2
                        3 Node 3
                        #
                        0 1 Edge 0
                        0 2 Edge 1
                        1 2 Edge 2
                        1 3 Edge 3
                        3 0 Edge 4
                        """
                    )
                )
            graph = tlp.importGraph(self.tgfPluginName, {"filename": tgfFile})
            self.assertEqual(
                [graph["viewLabel"][n] for n in graph.nodes()],
                [f"Node {i}" for i in range(4)],
            )
            self.assertEqual(
                [graph["viewLabel"][e] for e in graph.edges()],
                [f"Edge {i}" for i in range(5)],
            )

    def test_load_tgf_graph_invalid_edge(self):
        with tempfile.TemporaryDirectory() as tgfDir:
            tgfFile = os.path.join(tgfDir, "graph.tgf")
            with open(tgfFile, "w") as tgfFileWriter:
                tgfFileWriter.write(
                    textwrap.dedent(
                        """
                        0
                        1
                        2
                        3
                        #
                        4
                        """
                    )
                )
            with self.assertRaises(ValueError) as cm:
                tlp.importGraph(self.tgfPluginName, {"filename": tgfFile})
            self.assertEqual(
                cm.exception.args[0],
                "Invalid edge definition: 4"
            )

            with self.assertRaises(ValueError) as cm:
                tlp.loadGraph(tgfFile)
            self.assertEqual(
                cm.exception.args[0],
                "Invalid edge definition: 4"
            )

    def test_load_tgf_graph_invalid_node_id(self):
        with tempfile.TemporaryDirectory() as tgfDir:
            tgfFile = os.path.join(tgfDir, "graph.tgf")
            with open(tgfFile, "w") as tgfFileWriter:
                tgfFileWriter.write(
                    textwrap.dedent(
                        """
                        0
                        1
                        2
                        3
                        #
                        4 5
                        """
                    )
                )
            with self.assertRaises(ValueError) as cm:
                tlp.importGraph(self.tgfPluginName, {"filename": tgfFile})
            self.assertEqual(
                cm.exception.args[0],
                "Invalid node identifier found when parsing edges: 4"
            )

            with self.assertRaises(ValueError) as cm:
                tlp.loadGraph(tgfFile)
            self.assertEqual(
                cm.exception.args[0],
                "Invalid node identifier found when parsing edges: 4"
            )
