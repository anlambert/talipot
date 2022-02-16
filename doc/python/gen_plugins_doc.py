# automatically generates the file talipotpluginsdocumentation.rst
# by dynamically introspecting the Talipot plugins metadata

from __future__ import print_function
from talipot import tlp
import tabulate
import re
import os

talipot_build_dir = os.environ['TALIPOT_BUILD_DIR']
tlp.loadTalipotPluginsFromDir('%s/plugins/clustering' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/colors' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/export' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/general' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/import' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/layout' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/metric' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/selection' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/sizes' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/string' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir('%s/plugins/test' % talipot_build_dir)
tlp.loadTalipotPluginsFromDir(os.environ['TALIPOT_PYTHON_PLUGINS_DIR'])

f = open('talipotpluginsdocumentation.rst', 'w', encoding='utf-8')

print("""
.. |br| raw:: html

   <br />

.. |bstart| raw:: html

   <b>

.. |bend| raw:: html

   </b>

.. |istart| raw:: html

   <i>

.. |iend| raw:: html

   </i>

.. |listart| raw:: html

   <li>

.. |liend| raw:: html

   </li>

.. |ulstart| raw:: html

   <ul>

.. |ulend| raw:: html

   </ul>

.. |pstart| raw:: html

   <p>

.. |pend| raw:: html

   </p>

""", file=f)

print('.. py:currentmodule:: talipot\n', file=f)

print('.. _talipotpluginsdoc:\n', file=f)


def writeSection(title, sectionChar):
    print(title, file=f)
    underline = ''
    for i in range(len(title)):
        underline += sectionChar
    print(underline+'\n', file=f)


def replaceHtmlTags(doc, htmlTag, htmlTagStartReplacement,
                    htmlTagEndReplacement):
    tmp = re.sub('^<%s>' % htmlTag,
                 '|%s| ' % htmlTagStartReplacement, doc)
    tmp = re.sub('^</%s>' % htmlTag,
                 '|%s| ' % htmlTagEndReplacement, tmp)
    tmp = re.sub('^<%s>' % htmlTag.upper(),
                 '|%s| ' % htmlTagStartReplacement, tmp)
    tmp = re.sub('^</%s>' % htmlTag.upper(),
                 '|%s| ' % htmlTagEndReplacement, tmp)
    tmp = tmp.replace('<%s>' % htmlTag,
                      ' |%s| ' % htmlTagStartReplacement)
    tmp = tmp.replace('</%s>' % htmlTag,
                      ' |%s| ' % htmlTagEndReplacement)
    tmp = tmp.replace('<%s>' % htmlTag.upper(),
                      ' |%s| ' % htmlTagStartReplacement)
    tmp = tmp.replace('</%s>' % htmlTag.upper(),
                      ' |%s| ' % htmlTagEndReplacement)
    return tmp


def formatSphinxDoc(doc):
    doc = re.sub('^\n', '', doc)
    doc = re.sub('\n$', '', doc)
    doc = replaceHtmlTags(doc, 'br', 'br', 'br')
    doc = replaceHtmlTags(doc, 'br/', 'br', 'br')
    doc = replaceHtmlTags(doc, 'b', 'bstart', 'bend')
    doc = replaceHtmlTags(doc, 'i', 'istart', 'iend')
    doc = replaceHtmlTags(doc, 'li', 'listart', 'liend')
    doc = replaceHtmlTags(doc, 'ul', 'ulstart', 'ulend')
    doc = replaceHtmlTags(doc, 'p', 'pstart', 'pend')
    return doc


def getTalipotPythonType(talipotType):
    if talipotType == 'BooleanProperty':
        return ':class:`tlp.BooleanProperty`'
    elif talipotType == 'ColorProperty':
        return ':class:`tlp.ColorProperty`'
    elif talipotType == 'DoubleProperty':
        return ':class:`tlp.DoubleProperty`'
    elif talipotType == 'IntegerProperty':
        return ':class:`tlp.IntegerProperty`'
    elif talipotType == 'LayoutProperty':
        return ':class:`tlp.LayoutProperty`'
    elif talipotType == 'SizeProperty':
        return ':class:`tlp.SizeProperty`'
    elif talipotType == 'StringProperty':
        return ':class:`tlp.StringProperty`'
    elif talipotType == 'NumericProperty':
        return ':class:`tlp.NumericProperty`'
    elif talipotType == 'PropertyInterface':
        return ':class:`tlp.PropertyInterface`'
    elif talipotType == 'StringCollection':
        return ':class:`tlp.StringCollection`'
    elif talipotType == 'ColorScale':
        return ':class:`tlp.ColorScale`'
    elif talipotType == 'Color':
        return ':class:`tlp.Color`'
    elif talipotType == 'Boolean':
        return ':const:`bool`'
    elif talipotType.startswith('float'):
        return ':const:`float`'
    elif 'integer' in talipotType:
        return ':const:`int`'
    elif talipotType == 'string':
        return ':const:`str`'
    else:
        return talipotType


writeSection('Talipot plugins documentation', '=')

print("""
In this section, you can find some documentation regarding the C++ algorithm
plugins bundled in the Talipot software but also with the Talipot Python
modules installable through the pip tool. In particular, an exhaustive
description of the input and output parameters for each plugin is given.
To learn how to call all these algorithms in Python, you can refer to the
:ref:`Applying an algorithm on a graph <applyGraphAlgorithm>` section.
The plugins documentation is ordered according to their type.
""" + '\n', file=f)

pluginsNames = tlp.PluginsManager.availablePlugins()
plugins = {}
for pluginName in pluginsNames:
    plugin = tlp.PluginsManager.pluginInformation(pluginName)
    if not plugin.category() in plugins:
        plugins[plugin.category()] = []
    plugins[plugin.category()].append(plugin)
for cat in sorted(plugins.keys()):
    if cat in ('Panel', 'Node shape', 'Edge extremity', 'Interactor'):
        continue
    writeSection(cat, '-')
    if cat == 'Algorithm':
        print('.. _algorithmpluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':meth:`tlp.Graph.applyAlgorithm` method. See also '
               ':ref:`Calling a general algorithm on a graph '
               '<callGeneralAlgorithm>` for more details.\n'), file=f)
    elif cat == 'Coloring':
        print('.. _colorpluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':meth:`tlp.Graph.applyColorAlgorithm` method. See also '
               ':ref:`Calling a property algorithm on a graph '
               '<callPropertyAlgorithm>` for more details.\n'), file=f)
    elif cat == 'Export':
        print('.. _exportpluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':func:`tlp.exportGraph` function.\n'), file=f)
    elif cat == 'Import':
        print('.. _importpluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':func:`tlp.importGraph` function.\n'), file=f)
    elif cat == 'Labeling':
        print('.. _stringpluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':meth:`tlp.Graph.applyStringAlgorithm` method. See also '
               ':ref:`Calling a property algorithm on a graph '
               '<callPropertyAlgorithm>` for more details.\n'), file=f)
    elif cat == 'Layout':
        print('.. _layoutpluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':meth:`tlp.Graph.applyLayoutAlgorithm` method. See also '
               ':ref:`Calling a property algorithm on a graph '
               '<callPropertyAlgorithm>` for more details.\n'), file=f)
    elif cat == 'Measure':
        print('.. _doublepluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':meth:`tlp.Graph.applyDoubleAlgorithm` method. See also '
               ':ref:`Calling a property algorithm on a graph '
               '<callPropertyAlgorithm>` for more details.\n'), file=f)
    elif cat == 'Resizing':
        print('.. _sizepluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':meth:`tlp.Graph.applySizeAlgorithm` method. See also '
               ':ref:`Calling a property algorithm on a graph '
               '<callPropertyAlgorithm>` for more details.\n'), file=f)
    elif cat == 'Selection':
        print('.. _booleanpluginsdoc:\n', file=f)
        print(('To call these plugins, you must use the '
               ':meth:`tlp.Graph.applyBooleanAlgorithm` method. See also '
               ':ref:`Calling a property algorithm on a graph '
               '<callPropertyAlgorithm>` for more details.\n'), file=f)

    for p in sorted(plugins[cat], key=lambda p: p.name()):
        writeSection(p.name(), '^')
        writeSection('Description', '"')
        infos = formatSphinxDoc(p.info())
        print(infos+'\n', file=f)

        params = tlp.PluginsManager.getPluginParameters(p.name())
        headers = ["name", "type", "default", "direction", "description"]
        paramsTable = []
        nbInParams = 0
        for param in params.getParameters():
            paramHelpHtml = param.getHelp()
            pattern = '<p class="help">'
            pos = paramHelpHtml.find(pattern)
            paramHelp = ''
            if pos != -1:
                pos2 = paramHelpHtml.rfind('</p>')
                paramHelp = paramHelpHtml[pos+len(pattern):pos2]
                paramHelp = formatSphinxDoc(paramHelp)
            pattern = '<b>type</b><td class="b">'
            pos = paramHelpHtml.find(pattern)
            paramType = ''
            if pos != -1:
                pos2 = paramHelpHtml.find('</td>', pos)
                paramType = paramHelpHtml[pos+len(pattern):pos2].replace(
                    ' (double precision)', '')
            if param.getName() == 'result' and 'Property' in paramType:
                continue
            paramDir = 'input / output'
            if param.getDirection() == tlp.ParameterDirection.IN_PARAM:
                paramDir = 'input'
                nbInParams = nbInParams+1
            elif param.getDirection() == tlp.ParameterDirection.OUT_PARAM:
                paramDir = 'output'
            else:
                nbInParams = nbInParams+1
            pattern = '<b>values</b><td class="b">'
            pos = paramHelpHtml.find(pattern)
            paramValues = ''
            if pos != -1:
                pos2 = paramHelpHtml.find('</td>', pos)
                paramValues = paramHelpHtml[pos+len(pattern):pos2]
                paramValues = formatSphinxDoc(paramValues)
            paramDefValue = param.getDefaultValue()
            if paramType == 'StringCollection':
                paramDefValue = paramDefValue[0:paramDefValue.find(';')]
            else:
                paramDefValue = paramDefValue.replace(
                    'false', ':const:`False`')
                paramDefValue = paramDefValue.replace('true', ':const:`True`')
            nonBreakingSpace = u'\xa0'
            if len(paramValues) > 0:
                paramDefValue += (u' |br| |br| |bstart| Values: |bend| |br| %s'
                                  % paramValues)
            valuesDoc = False

            paramName = param.getName().replace(
                'file::', '').replace('dir::', '')
            paramName = paramName.replace(' ', nonBreakingSpace)
            paramType = getTalipotPythonType(
                paramType.replace(' ', nonBreakingSpace))
            paramDir = paramDir.replace(' ', nonBreakingSpace)
            paramsTable.append([paramName, paramType, paramDefValue, paramDir,
                                paramHelp])
        if len(paramsTable) > 0:
            writeSection('Parameters', '"')
            print(tabulate.tabulate(paramsTable, headers,
                                    tablefmt='grid') + '\n', file=f)
        writeSection('Calling the plugin from Python', '"')
        print(('To call that plugin from Python, use the following '
               'code snippet::\n'), file=f)
        print(('  # get a dictionary filled with the default plugin '
               'parameters values'), file=f)
        if p.category() != 'Import':
            print(('  # graph is an instance of the tlp.Graph class\n'
                   '  params = tlp.getDefaultPluginParameters(\'%s\', '
                   'graph)\n') % p.name(), file=f)
        else:
            print('  params = tlp.getDefaultPluginParameters(\'%s\')\n'
                  % p.name(), file=f)
        if nbInParams > 0 and not(len(paramsTable) == 1 and
                                  paramsTable[0][0] == 'result'):
            print('  # set any input parameter value if needed', file=f)
            for paramData in paramsTable:
                print('  # params[\'%s\'] = ...' % paramData[0], file=f)
            print('', file=f)
        if p.category() == 'Import':
            print('  graph = tlp.importGraph(\'%s\', params)' % p.name(),
                  file=f)
        elif p.category() == 'Export':
            print('  outputFile = \'<path to a file>\'', file=f)
            print(('  success = tlp.exportGraph(\'%s\', graph, outputFile,'
                   ' params)') % p.name(), file=f)
        elif p.category() == 'Layout':
            print(('  # either create or get a layout property from the'
                   ' graph to store the result of the algorithm'), file=f)
            print(('  resultLayout = graph.getLayoutProperty('
                   '\'resultLayout\')'), file=f)
            print(('  success = graph.applyLayoutAlgorithm(\'%s\', '
                   'resultLayout, params)\n') % p.name(), file=f)
            print(('  # or store the result of the algorithm in the '
                   'default Talipot layout property named \'viewLayout\''),
                  file=f)
            print(('  success = graph.applyLayoutAlgorithm(\'%s\', '
                   'params)\n') % p.name(), file=f)
        elif p.category() == 'Measure':
            print(('  # either create or get a double property from the'
                   ' graph to store the result of the algorithm'), file=f)
            print(('  resultMetric = graph.getDoubleProperty('
                   '\'resultMetric\')'), file=f)
            print(('  success = graph.applyDoubleAlgorithm(\'%s\''
                   ', resultMetric, params)\n') % p.name(), file=f)
            print(('  # or store the result of the algorithm in the '
                   'default Talipot metric property named \'viewMetric\''),
                  file=f)
            print(('  success = graph.applyDoubleAlgorithm(\'%s\','
                   ' params)\n') % p.name(), file=f)
        elif p.category() == 'Coloring':
            print(('  # either create or get a color property from the'
                  ' graph to store the result of the algorithm'), file=f)
            print(('  resultColor = graph.getColorProperty('
                  '\'resultColor\')'), file=f)
            print(('  success = graph.applyColorAlgorithm(\'%s\','
                  ' resultColor, params)\n') % p.name(), file=f)
            print(('  # or store the result of the algorithm in the'
                  ' default Talipot color property named \'viewColor\''),
                  file=f)
            print(('  success = graph.applyColorAlgorithm(\'%s\', '
                   'params)\n') % p.name(), file=f)
        elif p.category() == 'Resizing':
            print(('  # either create or get a size property from the'
                   ' graph to store the result of the algorithm'), file=f)
            print('  resultSize = graph.getSizeProperty(\'resultSize\')',
                  file=f)
            print(('  success = graph.applySizeAlgorithm(\'%s\','
                   ' resultSize, params)\n') % p.name(), file=f)
            print(('  # or store the result of the algorithm in the'
                   ' default Talipot size property named \'viewSize\''),
                  file=f)
            print(('  success = graph.applySizeAlgorithm(\'%s\', '
                   'params)\n') % p.name(), file=f)
        elif p.category() == 'Selection':
            print(('  # either create or get a boolean property from the'
                   ' graph to store the result of the algorithm'), file=f)
            print(('  resultSelection = graph.getBooleanProperty('
                   '\'resultSelection\')'), file=f)
            print(('  success = graph.applyBooleanAlgorithm(\'%s\', '
                   'resultSelection, params)\n') % p.name(), file=f)
            print(('  # or store the result of the algorithm in the '
                   'default Talipot boolean property named '
                   '\'viewSelection\''), file=f)
            print(('  success = graph.applyBooleanAlgorithm(\'%s\', '
                   'params)\n') % p.name(), file=f)
        elif p.category() == 'Labeling':
            print(('  # either create or get a string property from the '
                   'graph to store the result of the algorithm'), file=f)
            print(('  resultString = graph.getStringProperty('
                   '\'resultString\')'), file=f)
            print(('  success = graph.applyStringAlgorithm(\'%s\', '
                   'resultString, params)\n') % p.name(), file=f)
            print(('  # or store the result of the algorithm in the '
                   'default Talipot string property named \'viewLabel\''),
                  file=f)
            print(('  success = graph.applyStringAlgorithm(\'%s\', '
                  'params)\n') % p.name(), file=f)
        elif p.category() == 'Algorithm':
            print(('  success = graph.applyAlgorithm(\'%s\', '
                   'params)\n') % p.name(), file=f)
        print(('  # if the plugin declare any output parameter, its value'
               ' can now be retrieved in the \'params\' dictionary\n'),
              file=f)
f.close()
