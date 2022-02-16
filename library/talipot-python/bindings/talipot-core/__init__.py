# Copyright (C) 2019-2022  The Talipot developers
#
# Talipot is a fork of Tulip, created by David Auber
# and the Tulip development Team from LaBRI, University of Bordeaux
#
# See the AUTHORS file at the top-level directory of this distribution
# License: GNU General Public License version 3, or any later version
# See top-level LICENSE file for more information

# -*- coding: utf-8 -*-

import os
import os.path
import sys
import traceback
import platform

_talipotNativeLibsPath = os.path.join(os.path.dirname(__file__), 'native')
sys.path.append(_talipotNativeLibsPath)

if platform.system() == 'Windows':
    os.environ['PATH'] = '%s;%s;%s' % (
        _talipotNativeLibsPath,
        os.path.join(_talipotNativeLibsPath, '../../..'),
        os.environ['PATH'])

import _talipot # noqa

sys.path.pop()


class tlpType(_talipot.tlp.__class__):

    def __getattr__(cls, name):
        if hasattr(_talipot.tlp, name):
            return _talipot.tlp.getGlobalVar(name)
        else:
            raise AttributeError(name)

    def __setattr__(cls, name, value):
        if hasattr(_talipot.tlp, name):
            _talipot.tlp.setGlobalVar(name, value)
        else:
            super(tlpType, cls).__setattr__(name, value)


# utility function from the 'six' module
def with_metaclass(meta, *bases):
    """Create a base class with a metaclass."""
    # This requires a bit of explanation: the basic idea is to make a dummy
    # metaclass for one level of class instantiation that replaces itself with
    # the actual metaclass.
    class metaclass(meta):
        def __new__(cls, name, this_bases, d):
            return meta(name, bases, d)

    return type.__new__(metaclass, 'temporary_class', (), {})


class tlp(with_metaclass(tlpType, _talipot.tlp)):

    @staticmethod
    def loadTalipotPythonPlugin(pluginFilePath):
        if not os.path.isfile(pluginFilePath):
            print('[talipot] Error: Path %s is not a valid file' %
                  pluginFilePath)
            return False

        try:
            with open(pluginFilePath) as pluginFile:
                pluginFileContent = pluginFile.read()
        except Exception:
            return False

        if 'talipotplugins.register' not in pluginFileContent:
            return False

        modulePath = os.path.dirname(pluginFilePath)
        moduleName = os.path.basename(pluginFilePath)[:-3]

        if modulePath not in sys.path:
            sys.path.append(modulePath)

        try:
            __import__(moduleName)
        except ImportError:
            sys.stdout.write(('There was an error when trying to load the'
                              ' Talipot Python plugin from the file %s. See '
                              'stack trace below.\\n') % pluginFilePath)
            traceback.print_exc()
            return False

        return True

    @staticmethod
    def loadTalipotPluginsFromDir(pluginsDirPath, loadCppPlugin=True,
                                  pluginLoader=None):
        if not os.path.exists(pluginsDirPath):
            return False

        if loadCppPlugin:
            tlp.loadPluginsFromDir(pluginsDirPath, pluginLoader, False)

        files = os.listdir(pluginsDirPath)

        for file in files:
            filePath = os.path.join(pluginsDirPath, file)
            if not os.path.isdir(filePath) and filePath.endswith('.py'):
                tlp.loadTalipotPythonPlugin(filePath)

        for file in files:
            filePath = os.path.join(pluginsDirPath, file)
            if os.path.isdir(filePath):
                tlp.loadTalipotPluginsFromDir(filePath, loadCppPlugin,
                                              pluginLoader)

        return True


talipotVersion = tlp.getRelease()
talipotVersion = talipotVersion[:talipotVersion.rfind('.')]

startupScriptsPath = os.path.join(
    tlp.TalipotLibDir, 'talipot/python/startup')
startupScriptsHomePath = os.path.join(
    os.path.expanduser('~'), '.Talipot-%s/python/startup' % talipotVersion)


def runStartupScripts(scriptsPath):
    if not os.path.exists(scriptsPath):
        return

    files = os.listdir(scriptsPath)

    for file in files:
        filePath = os.path.join(scriptsPath, file)
        if os.path.isfile(filePath) and filePath.endswith('.py'):
            with open(filePath) as fd:
                exec(compile(fd.read(), filePath, 'exec'),
                     globals(), locals())


runStartupScripts(startupScriptsPath)
runStartupScripts(startupScriptsHomePath)

tlpPythonPluginsPath = os.path.join(
    tlp.TalipotLibDir, 'talipot/python/talipot/plugins')
tlpPythonPluginsHomePath = os.path.join(
    os.path.expanduser('~'), '.Talipot-%s/plugins/python' % talipotVersion)

tlp.loadTalipotPluginsFromDir(tlpPythonPluginsPath, False)
tlp.loadTalipotPluginsFromDir(tlpPythonPluginsHomePath, False)

_talipotNativePluginsPath = os.path.join(_talipotNativeLibsPath, 'plugins')

# fix loading of Talipot plugins when the talipot module has been
# installed with the pip tool
if platform.system() == 'Linux' and os.path.exists(_talipotNativePluginsPath):
    dlOpenFlagsBackup = sys.getdlopenflags()
    if sys.version_info < (3, 6):
        import DLFCN
        dlOpenFlags = DLFCN.RTLD_NOW | DLFCN.RTLD_GLOBAL
    else:
        dlOpenFlags = os.RTLD_NOW | os.RTLD_GLOBAL
    sys.setdlopenflags(dlOpenFlags)

tlp.loadTalipotPluginsFromDir(_talipotNativePluginsPath)

# load bundled Talipot Python plugins when the talipot module has been
# installed with the pip tool
if not sys.argv[0] == 'talipot':
    tlp.loadTalipotPluginsFromDir(
        os.path.join(os.path.dirname(__file__), 'plugins'))

if platform.system() == 'Linux' and os.path.exists(_talipotNativePluginsPath):
    sys.setdlopenflags(dlOpenFlagsBackup)

__all__ = ['tlp']
__author__ = 'The Talipot developers'
__license__ = 'GPLv3'
__version__ = tlp.getRelease()
__status__ = 'Production'
