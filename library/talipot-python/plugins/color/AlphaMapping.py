# Copyright (C) 2019-2023  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

# Talipot Python plugin for mapping metric values to alpha component
# of graph element colors. In other words, it enables to compute
# the graph elements transparency according to the values
# stored in a numeric property of a graph.

import math

import talipotplugins

from talipot import tlp


def clamp(n, smallest, largest):
    return max(smallest, min(n, largest))


def getFactor(v, r):
    if r == 0:
        r = 1
    if v < 0:
        v = 0
    if v > r:
        v = r
    return v / r


mappingTypeDoc = """
That parameter defines the type of alpha mapping to perform.
For the linear case, the minimum value of the input numeric property
is mapped to a minimum alpha value picked by the user, the maximum
value is mapped to a maximum alpha value picked by the user, and a
linear interpolation is used between both to compute the associated
alpha of the graph element color.<br/>
For the logarithmic case, input numeric properties values are first
mapped in the [1, +inf[ range. Then the log of each mapped value is
computed and used to compute the associated alpha value of the graph
element color through a linear interpolation between 0 and the log of
the mapped maximum value of graph elements.<br/>
If uniform, this is the same except for the interpolation: the values
are sorted, numbered, and a linear interpolation is used on those numbers
(in other words, only the order is taken into account, not the actual values).
"""


class AlphaMapping(tlp.ColorAlgorithm):

    def __init__(self, context):
        tlp.ColorAlgorithm.__init__(self, context)

        self.addColorPropertyParameter(
            'colors',
            'The input color property from which to compute new alpha values',
            'viewColor')

        self.addNumericPropertyParameter(
            'metric',
            'The input numeric property from which to compute alpha mapping',
            'viewMetric')

        self.addStringCollectionParameter(
            'target',
            'Whether alpha values are computed for nodes, edges or both',
            'nodes;edges;both')

        self.addStringCollectionParameter(
            'type', mappingTypeDoc, 'linear;logarithmic;uniform')

        self.addUnsignedIntegerParameter(
            'min alpha',
            ('The minimum alpha value (between 0 and 255) '
             'to map on graph elements colors'), '0')

        self.addUnsignedIntegerParameter(
            'max alpha',
            ('The maximum alpha value (between 0 and 255) '
             'to map on graph elements colors'), '255')

    def run(self):
        colors = self.dataSet['colors']
        self.result.copy(colors)

        inputMetric = self.dataSet['metric']
        minAlpha = clamp(self.dataSet['min alpha'], 0, 255)
        maxAlpha = clamp(self.dataSet['max alpha'], 0, 255)
        target = self.dataSet['target']
        mappingType = self.dataSet['type']

        if minAlpha > maxAlpha:
            minAlpha, maxAlpha = maxAlpha, minAlpha

        if mappingType == 'uniform':
            inputMetric = inputMetric.copyProperty(self.graph)
            inputMetric.uniformQuantification(300)

        elif mappingType == 'logarithmic':
            inputMetric = inputMetric.copyProperty(self.graph)
            minN = inputMetric.getNodeDoubleMin(self.graph)
            minE = inputMetric.getEdgeDoubleMin(self.graph)
            for n in self.graph.nodes():
                inputMetric[n] = math.log(1+inputMetric[n]-minN)
            for e in self.graph.edges():
                inputMetric[e] = math.log(1+inputMetric[e]-minE)

        if target in ('nodes', 'both'):
            minValue = inputMetric.getNodeDoubleMin(self.graph)
            maxValue = inputMetric.getNodeDoubleMax(self.graph)

            for n in self.graph.nodes():
                val = inputMetric[n]
                t = getFactor(val - minValue, maxValue - minValue)
                alpha = minAlpha + t * (maxAlpha - minAlpha)
                color = colors[n]
                color[3] = int(alpha)
                self.result[n] = color

        if target in ('edges', 'both'):
            minValue = inputMetric.getEdgeDoubleMin(self.graph)
            maxValue = inputMetric.getEdgeDoubleMax(self.graph)

            for e in self.graph.edges():
                val = inputMetric[e]
                t = getFactor(val - minValue, maxValue - minValue)
                alpha = minAlpha + t * (maxAlpha - minAlpha)
                color = colors[e]
                color[3] = int(alpha)
                self.result[e] = color

        return True


pluginDoc = """
Map metric values to alpha component of graph element colors.
In other words, it enables to compute the graph elements transparency
according to the values stored in a numeric property of a graph.
"""

# The line below does the magic to register the plugin into the plugin database
# and updates the GUI to make it accessible through the menus.
talipotplugins.registerPlugin('AlphaMapping', 'Alpha Mapping', 'Antoine Lambert',
                              '20/04/2017', pluginDoc, '1.2')
