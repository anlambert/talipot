# Copyright (C) 2020-2023  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

import os
import unittest

from talipot import tlp


class TestTalipotPlugins(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if "TALIPOT_BUILD_DIR" in os.environ:
            talipot_build_dir = os.environ["TALIPOT_BUILD_DIR"]
            if talipot_build_dir:
                for pluginsPath in (
                    "plugins/clustering",
                    "plugins/general",
                    "plugins/import",
                    "plugins/metric",
                    "library/talipot-python",
                ):
                    tlp.loadPluginsFromDir(os.path.join(talipot_build_dir, pluginsPath))

    def test_cpp_plugins_loaded(self):
        self.assertTrue("Edge bundling" in tlp.getAlgorithmPluginsList())

    def test_pythons_plugins_loaded(self):
        self.assertTrue("H3" in tlp.getLayoutAlgorithmPluginsList())

    def test_edge_bundling_3d_layout(self):
        # import a grid graph drawn in the plane
        graph = tlp.importGraph("Grid")
        # create a clone subgraph
        clone = graph.addCloneSubGraph()
        # add two nodes with z coordinate different from 0
        graph.addNode({"viewLayout": (-100, -100, -10)})
        graph.addNode({"viewLayout": (100, 100, 10)})
        # apply edge bundling on the clone graph
        params = tlp.getDefaultPluginParameters("Edge bundling", clone)
        ret = clone.applyAlgorithm("Edge bundling", params)
        # check that the resulting layout is in the plane
        self.assertTrue(ret[0])
        edge_bends = [graph["viewLayout"][e] for e in graph.getEdges()]
        for bends in edge_bends:
            for c in bends:
                self.assertEqual(c[2], 0)
        # apply edge bundling on the root graph
        ret = graph.applyAlgorithm("Edge bundling", params)
        # check that the layout failed as the 3D_layout parameter
        # has not been set to true
        self.assertFalse(ret[0])

    def test_alpha_mapping(self):
        nodeColor = (255, 0, 0, 255)
        nodeColorAfter = (255, 0, 0, 0)
        edgeColor = (0, 255, 0, 255)
        edgeColorAfter = (0, 255, 0, 0)
        graph = tlp.importGraph("Grid")
        graph["viewMetric"].setAllNodeValue(0)
        graph["viewMetric"].setAllEdgeValue(0)
        graph["viewColor"].setAllNodeValue(nodeColor)
        graph["viewColor"].setAllEdgeValue(edgeColor)

        # check for viewColor output property
        params = tlp.getDefaultPluginParameters("Alpha Mapping", graph)
        params["target"] = "nodes"
        graph.applyColorAlgorithm("Alpha Mapping", params)

        for n in graph.nodes():
            self.assertEqual(graph["viewColor"][n], nodeColorAfter)

        for e in graph.edges():
            self.assertEqual(graph["viewColor"][e], edgeColor)

        graph["viewColor"].setAllNodeValue(nodeColor)
        graph["viewColor"].setAllEdgeValue(edgeColor)

        params["target"] = "edges"
        graph.applyColorAlgorithm("Alpha Mapping", params)

        for n in graph.nodes():
            self.assertEqual(graph["viewColor"][n], nodeColor)

        for e in graph.edges():
            self.assertEqual(graph["viewColor"][e], edgeColorAfter)

        colors = tlp.ColorProperty(graph)
        colors.setAllNodeValue(tlp.Color.White)
        colors.setAllEdgeValue(tlp.Color.Black)

        params["colors"] = colors
        params["target"] = "both"
        graph.applyColorAlgorithm("Alpha Mapping", params)

        for n in graph.nodes():
            self.assertEqual(graph["viewColor"][n], (255, 255, 255, 0))

        for e in graph.edges():
            self.assertEqual(graph["viewColor"][e], (0, 0, 0, 0))

        # check for anonymous or not viewColor output property
        colorProps = [tlp.ColorProperty(graph), graph.getColorProperty("colors")]

        for colorProp in colorProps:
            colorProp.setAllNodeValue(tlp.Color.White)
            colorProp.setAllEdgeValue(tlp.Color.Black)

            graph["viewColor"].setAllNodeValue(nodeColor)
            graph["viewColor"].setAllEdgeValue(edgeColor)

            params["colors"] = graph["viewColor"]
            params["target"] = "nodes"
            graph.applyColorAlgorithm("Alpha Mapping", colorProp, params)

            for n in graph.nodes():
                self.assertEqual(colorProp[n], nodeColorAfter)

            for e in graph.edges():
                self.assertEqual(colorProp[e], edgeColor)

            graph["viewColor"].setAllNodeValue(nodeColor)
            graph["viewColor"].setAllEdgeValue(edgeColor)

            params["target"] = "edges"
            graph.applyColorAlgorithm("Alpha Mapping", colorProp, params)

            for n in graph.nodes():
                self.assertEqual(colorProp[n], nodeColor)

            for e in graph.edges():
                self.assertEqual(colorProp[e], edgeColorAfter)

    def random_general_tree_test(self, min_size, max_size, max_degree):
        plugin_name = "Random General Tree"
        params = tlp.getDefaultPluginParameters(plugin_name)
        params["Minimum size"] = min_size
        params["Maximum size"] = max_size
        params["Maximal node's degree"] = max_degree

        graph = tlp.importGraph(plugin_name, params)

        self.assertTrue(graph.numberOfNodes() == graph.numberOfEdges() + 1)
        self.assertTrue(graph.numberOfNodes() >= min_size)
        self.assertTrue(graph.numberOfNodes() <= max_size)

        self.assertTrue(all(graph.outdeg(n) <= max_degree for n in graph.nodes()))

    def test_random_general_tree(self):
        self.random_general_tree_test(min_size=1000, max_size=2000, max_degree=20)

    def test_random_general_tree_fixed_size(self):
        self.random_general_tree_test(min_size=1000, max_size=1000, max_degree=5)

    def test_cluster_metric(self):
        graph = tlp.importGraph("Grid")
        params = tlp.getDefaultPluginParameters("Cluster", graph)
        graph.applyDoubleAlgorithm("Cluster", params)
        for n in graph.nodes():
            m = graph.deg(n) + 1
            assert graph["viewMetric"][n] == graph.deg(n) / ((m * (m - 1)) / 2)
