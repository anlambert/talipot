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

#include <talipot/PythonVersionChecker.h>

#include <QProcess>
#include <QRegularExpression>

// Current Python versions
static const char *pythonVersion[] = {"3.12", "3.11", "3.10", "3.9", "3.8", "3.7", "3.6",
                                      "3.5",  "3.4",  "3.3",  "3.2", "3.1", "3.0", nullptr};

// Windows specific functions
#ifdef WIN32

#include <QFileInfo>
#include <QSettings>

#include <windows.h>

#ifndef X86_64
// function to check if a 32 bits program run on a 64 bits system
static bool isWow64() {
  BOOL bIsWow64 = FALSE;

  typedef BOOL(APIENTRY * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

  LPFN_ISWOW64PROCESS fnIsWow64Process;

  HMODULE module = GetModuleHandle(TEXT("kernel32"));
  const char funcName[] = "IsWow64Process";
  fnIsWow64Process = LPFN_ISWOW64PROCESS(GetProcAddress(module, funcName));

  if (fnIsWow64Process != nullptr) {
    fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
  }

  return bIsWow64 != FALSE;
}
#endif

#ifndef MSYS2_PYTHON
// Check if a path is a valid Python Home, meaning it is not empty and contains the python
// executable
static bool validPythonHome(const QString &pythonHome) {
  return !pythonHome.isEmpty() && QFileInfo(pythonHome + "/python.exe").exists();
}
#endif

// Function to get the path to Python home directory for a specific Python version.
// Returns an empty string if the provided version is not installed on the host system.
// The path to the Python home directory is retrieved from the windows registry.
// On windows, Python can be installed for all users or for the current user only. That function
// handles both cases.
// The current user installation will be preferred over the all users one.
static QString pythonHome(const QString &pythonVersion) {

// special case when using Python provided by MSYS2
#ifdef MSYS2_PYTHON
  std::ignore = pythonVersion;
  return PYTHON_HOME_PATH;

// standard Python installation on Windows
#else

  QString winRegKeyAllUsers, winRegKeyCurrentUser;

  // 32 bit Python
#ifndef X86_64

  // on windows 64 bit
  if (isWow64()) {
    winRegKeyAllUsers = QString("HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Python\\PythonCore\\") +
                        pythonVersion + QString("\\InstallPath");
    winRegKeyCurrentUser =
        QString("HKEY_CURRENT_USER\\SOFTWARE\\Wow6432Node\\Python\\PythonCore\\") + pythonVersion +
        QString("\\InstallPath");
  }
  // on windows 32 bit
  else {
    winRegKeyAllUsers = QString("HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\") +
                        pythonVersion + QString("\\InstallPath");
    winRegKeyCurrentUser = QString("HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\") +
                           pythonVersion + QString("\\InstallPath");
  }

  // 64 bit Python
#else

  winRegKeyAllUsers = QString("HKEY_LOCAL_MACHINE\\SOFTWARE\\Python\\PythonCore\\") +
                      pythonVersion + QString("\\InstallPath");
  winRegKeyCurrentUser = QString("HKEY_CURRENT_USER\\SOFTWARE\\Python\\PythonCore\\") +
                         pythonVersion + QString("\\InstallPath");

#endif

  QSettings winSettingsAllUsers(winRegKeyAllUsers, QSettings::NativeFormat);
  QSettings winSettingsCurrentUser(winRegKeyCurrentUser, QSettings::NativeFormat);
  QString pythonHomeAllUsers = winSettingsAllUsers.value("Default").toString().replace("\\", "/");
  QString pythonHomeCurrentUser =
      winSettingsCurrentUser.value("Default").toString().replace("\\", "/");

  if (validPythonHome(pythonHomeCurrentUser)) {
    return pythonHomeCurrentUser;
  } else if (validPythonHome(pythonHomeAllUsers)) {
    return pythonHomeAllUsers;
  } else {
    return "";
  }

#endif
}

// Linux and Mac OS specific
#else

// Function which tries to run a specific version of the python interpreter.
static bool runPython(const QString &version) {
  QProcess pythonProcess;
  pythonProcess.start(QString("python") + version, QStringList() << "--version");
  return pythonProcess.waitForFinished(-1) && pythonProcess.exitStatus() == QProcess::NormalExit;
}

#endif

#ifndef WIN32
// Function to get the default Python version if any by running the python process.
static QString getDefaultPythonVersionIfAny() {
  QString defaultPythonVersion;
  QProcess pythonProcess;

  QString pythonCommand = "python";

  // Before Python 3.4, the version number was printed on the standard error output.
  // Starting Python 3.4 the version number is printed on the standard output.
  // So merge the output channels of the process.
  pythonProcess.setProcessChannelMode(QProcess::MergedChannels);
  pythonProcess.setReadChannel(QProcess::StandardOutput);
  pythonProcess.start(pythonCommand, QStringList() << "--version");
  pythonProcess.waitForFinished(-1);

  if (pythonProcess.exitStatus() == QProcess::NormalExit) {

    QString result = pythonProcess.readAll();

    QRegularExpression versionRegexp(".*([0-9]*\\.[0-9]*)\\..*");
    QRegularExpressionMatch match;

    if (result.indexOf(versionRegexp, 0, &match) != -1) {
      defaultPythonVersion = match.captured(1);

      // Check the binary type of the python executable (32 or 64 bits)
      pythonProcess.start(
          pythonCommand,
          QStringList()
              << "-c"
              << "import struct;import sys;sys.stdout.write(str(struct.calcsize('P')*8))");
      pythonProcess.waitForFinished(-1);
      QString arch = pythonProcess.readAll();

#ifdef X86_64

      if (arch != "64") {
        defaultPythonVersion = "";
      }

#else

      if (arch != "32") {
        defaultPythonVersion = "";
      }

#endif
    }
  }

  return defaultPythonVersion;
}
#endif

QStringList tlp::PythonVersionChecker::_installedVersions;
bool tlp::PythonVersionChecker::_installedVersionsChecked(false);

QStringList tlp::PythonVersionChecker::installedVersions() {

  if (!_installedVersionsChecked) {

// On Linux and Mac OS, we check the presence of Python by trying to
// run the interpreter on a separate process
#ifndef WIN32

    int i = 0;

    // Try to run pythonX.Y executable
    while (pythonVersion[i]) {
      if (runPython(pythonVersion[i])) {
        _installedVersions.append(pythonVersion[i]);
      }

      ++i;
    }

    // Also try to run python executable
    QString defaultPythonVersion = getDefaultPythonVersionIfAny();

    if (!defaultPythonVersion.isEmpty() && !_installedVersions.contains(defaultPythonVersion)) {
      _installedVersions.append(defaultPythonVersion);
    }

// On windows, we check the presence of Python by looking into the registry
#else

    int i = 0;

    while (pythonVersion[i]) {
      if (!pythonHome(pythonVersion[i]).isEmpty()) {
        _installedVersions.append(pythonVersion[i]);
      }

      ++i;
    }

#endif

    _installedVersionsChecked = true;
  }

  return _installedVersions;
}

QString tlp::PythonVersionChecker::compiledVersion() {
  return TLP_PYTHON;
}

bool tlp::PythonVersionChecker::isPythonVersionMatching() {
  return installedVersions().contains(compiledVersion());
}

#ifdef WIN32
QString tlp::PythonVersionChecker::getPythonHome() {
  if (isPythonVersionMatching()) {
    QString pythonHomeDir = pythonHome(compiledVersion());
// This is a hack for MinGW to allow the debugging of Tulip through GDB when compiled with Python
// 3.X installed in a non standard way.
#ifdef __MINGW32__

    if (pythonHomeDir.isEmpty()) {
      char *pythonDirEv = getenv("PYTHONDIR");

      if (pythonDirEv) {
        return QString(pythonDirEv);
      }
    }

#endif
    return pythonHomeDir;
  }

  return "";
}
#endif
