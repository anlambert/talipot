# Copyright (C) 2019-2023  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

"""
This module provides utility functions to register Talipot plugins written
in Python in the plugins database.
"""

import atexit
import sys
import traceback
from importlib import reload  # noqa

pluginFactory = {}
pluginModules = {}
testMode = False


def setTestMode(mode):
    global testMode
    testMode = mode


def getCallingModuleName():
    import sys
    frames = list(sys._current_frames().values())
    for i in range(len(frames)):
        f = frames[i]
        if f.f_globals['__name__'] == 'talipotplugins':
            f = f.f_back
            break
    while f.f_globals['__name__'] == 'talipotplugins':
        f = f.f_back
    return f.f_globals['__name__']


def reloadTalipotPythonPlugin(pluginName):
    if pluginName in pluginModules:
        module = pluginModules[pluginName]
        code = 'import %s\n' % module
        code += 'reload(%s)\n' % module
        exec(code)


def reloadTalipotPythonPlugins():
    for plugin in pluginModules.keys():
        reloadTalipotPythonPlugin(plugin)


def removePlugin(pluginName):
    from talipot import tlp
    if tlp.PluginsManager.pluginExists(pluginName):
        tlp.PluginsManager.removePlugin(pluginName)


def destroyPlugins():
    global pluginFactory
    for pluginName in pluginFactory.keys():
        removePlugin(pluginName)


# register an exit hook to remove sip Talipot Python plugin wrappers
# from the plugins database to avoid a segfault when the Talipot C++
# runtime terminates
atexit.register(destroyPlugins)


def initFactory(self):
    from talipot import tlp
    tlp.FactoryInterface.__init__(self)
    self.registerPlugin()


def runPlugin(plugin):
    ret = False
    try:
        ret = plugin.real_run()
    except Exception:
        if plugin.pluginProgress:
            plugin.pluginProgress.setError(traceback.format_exc())
        # Case where the plugin execution has not been launched through the
        # Talipot GUI, so print the stack trace to stderr
        from talipot import tlp
        if type(plugin.pluginProgress) == tlp.SimplePluginProgress:
            sys.stdout.write(('There was an error when running Python plugin '
                              'named "%s". See stack trace below.\n') %
                             plugin.name())
            traceback.print_exc()
    return ret


def importGraph(plugin):
    ret = False
    try:
        ret = plugin.real_importGraph()
    except Exception:
        if plugin.pluginProgress:
            plugin.pluginProgress.setError(traceback.format_exc())
        # Case where the plugin execution has not been launched through
        # the Talipot GUI, so print the stack trace to stderr
        from talipot import tlp
        if type(plugin.pluginProgress) == tlp.SimplePluginProgress:
            sys.stdout.write(('There was an error when running Python plugin '
                              'named "%s". See stack trace below.\n') %
                             plugin.name())
            traceback.print_exc()
    return ret


def exportGraph(plugin, os):
    ret = False
    try:
        ret = plugin.real_exportGraph(os)
    except Exception:
        if plugin.pluginProgress:
            plugin.pluginProgress.setError(traceback.format_exc())
        # Case where the plugin execution has not been launched through
        # the Talipot GUI, so print the stack trace to stderr
        from talipot import tlp
        if type(plugin.pluginProgress) == tlp.SimplePluginProgress:
            sys.stdout.write(('There was an error when running Python plugin '
                              'named "%s". See stack trace below.\n') %
                             plugin.name())
            traceback.print_exc()
    return ret


def createPlugin(context, pluginModule, pluginClassName, pluginName,
                 author, date, info, release, group):
    plugin = eval('%s.%s(context)' % (pluginModule, pluginClassName),
                  globals(), locals())
    if hasattr(plugin, 'run'):
        plugin.real_run = plugin.run
        plugin.run = lambda: runPlugin(plugin)
    elif hasattr(plugin, 'importGraph'):
        plugin.real_importGraph = plugin.importGraph
        plugin.importGraph = lambda: importGraph(plugin)
    elif hasattr(plugin, 'exportGraph'):
        plugin.real_exportGraph = plugin.exportGraph
        plugin.exportGraph = lambda os: exportGraph(plugin, os)
    plugin.name = lambda: pluginName
    plugin.group = lambda: group
    plugin.author = lambda: author
    plugin.date = lambda: date
    plugin.info = lambda: info
    plugin.release = lambda: release
    from talipot import tlp
    plugin.talipotRelease = lambda: tlp.getRelease()
    plugin.programmingLanguage = lambda: 'Python'
    return plugin


def registerPluginOfGroup(pluginClassName, pluginName, author, date, info,
                          release, group):
    """
    talipotplugins.registerPluginOfGroup(pluginClassName, pluginName, author, date, info, release, group)

    Registers a Talipot plugin written in Python in the plugins database and
    inserts it in a specific group.

    :param pluginClassName: the name of the Python class implementing the plugin
    :type pluginClassName: string
    :param pluginName: the name of the plugin as it will appear in the interface
        and that will be used to call it
    :type pluginName: string
    :param author: the name of the plugin's author
    :type author: string
    :param date: the date of creation of the plugin
    :type date: string
    :param info: some information relative to the plugin
    :type info: string
    :param release: the version number of the plugin in the form X.Y
    :type release: string
    :param group: the name of the group in which the plugin will be inserted
    :type group: string
    """ # noqa

    global pluginFactory
    removePlugin(pluginName)
    pluginModule = getCallingModuleName()
    try:
        globals()[pluginModule] = __import__(pluginModule, globals(), locals(),
                                             [], 0)
        eval('%s.%s' % (pluginModule, pluginClassName), globals(), locals())
        if testMode:
            return
        pluginModules[pluginName] = pluginModule
        from talipot import tlp
        pluginFactory[pluginName] = type(
            '%sFactory' % pluginClassName, (tlp.FactoryInterface,),
            {
                '__init__': (lambda self: initFactory(self)),
                'createPluginObject': (lambda self, context: createPlugin(
                    context, pluginModule, pluginClassName, pluginName, author,
                    date, info, release, group))
            }
        )()

    except Exception:
        sys.stdout.write(('There was an error when registering Python plugin '
                          'named "%s". See stack trace below.\n') %
                         pluginName)
        raise


def registerPlugin(pluginClassName, pluginName, author, date, info, release):
    """
    talipotplugins.registerPlugin(pluginClassName, pluginName, author, date, info, release)

    Registers a Talipot plugin written in Python in the plugins database.

    :param pluginClassName: the name of the Python class implementing
        the plugin
    :type pluginClassName: string
    :param pluginName: the name of the plugin as it will appear in the
        interface and that will be used to call it
    :type pluginName: string
    :param author: the name of the plugin's author
    :type author: string
    :param date: the date of creation of the plugin
    :type date: string
    :param info: some information relative to the plugin
    :type info: string
    :param release: the version number of the plugin in the form X.Y
    :type release: string
    """ # noqa

    registerPluginOfGroup(pluginClassName, pluginName, author, date, info,
                          release, '')
