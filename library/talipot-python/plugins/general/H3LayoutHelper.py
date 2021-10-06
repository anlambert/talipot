# Copyright (C) 2019-2021  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

from talipot import tlp
import talipotplugins


class H3LayoutHelper(tlp.Algorithm):
    def __init__(self, context):
        tlp.Algorithm.__init__(self, context)
        self.addFloatParameter(
            'layout scaling',
            'the scale factor to apply to the computed layout',
            '1000')

    def check(self):
        if not tlp.ConnectedTest.isConnected(self.graph):
            return (False, 'The graph must be connected')

        return (True, '')

    def run(self):

        scaling = 1000
        if self.dataSet:
            scaling = self.dataSet['layout scaling']

        params = tlp.getDefaultPluginParameters('H3', self.graph)
        params['layout scaling'] = scaling
        success, errmsg = self.graph.applyLayoutAlgorithm('H3', params)
        if not success:
            if self.pluginProgress:
                self.pluginProgress.setError(errmsg)
                return False

        self.graph['viewBorderWidth'].setAllNodeValue(0)
        self.graph['viewShape'].setAllNodeValue(tlp.NodeShape.Sphere)
        self.graph['viewTgtAnchorShape'].setAllEdgeValue(
            tlp.EdgeExtremityShape.Cone)

        for n in self.graph.getNodes():
            w = self.graph['viewSize'][n][0]
            h = self.graph['viewSize'][n][1]
            s = min(w, h)
            self.graph['viewSize'][n] = (s, s, s)

        return True


pluginDoc = """
Enables to easily configure a H3 layout visualisation for a connected
quasi-hierarchical graph. As this is a 3d layout, some rendering setup
has to be done after the algorithm execution in order to get an aesthetic
rendering of it in Talipot. That plugin takes care of calling the H3 layout
algorithm, setting node shapes as sphere, setting edge extremity shapes to
cone and set appropriate rendering parameters for 3d layout visualization.
"""

# The line below does the magic to register the plugin into the plugin database
# and updates the GUI to make it accessible through the menus.
talipotplugins.registerPluginOfGroup(
    'H3LayoutHelper', 'H3 Layout Helper', 'Antoine Lambert', '01/09/2017',
    pluginDoc, '1.0', 'Misc')
