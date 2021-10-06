.. py:currentmodule:: talipot

Getting started
===============

.. _usingBindingsInTalipotGUI:

Using the bindings from the Talipot Software GUI
------------------------------------------------

Talipot Python bindings were primiraly designed to add a scripting feature to the main Talipot software and thus
interactively manipulate the graphs loaded into it through a couple lines of Python code.
In that purpose, several GUI components have been developed with the Qt framework and are available
through the main Talipot interface:

  * a **Python REPL** (Read Eval Print Loop) panel
  * a **Python IDE** (Integrated Development Environment) window

Python REPL
^^^^^^^^^^^

The **Python REPL** (Read Eval Print Loop) panel is accessible through the "Python REPL" button
at the bottom of the Talipot GUI (see :ref:`Figure 1<fig1>`). It enables to execute Python statements
in an interactive manner. A combo box allows to select a graph from those currently loaded in the
Talipot software. The selected graph is then bound to a global Python variable named "graph".
The undo / redo features on graph state is now handled in that component, meaning
every graph modification performed by the executed Python statements can be reverted.

.. _fig1:
.. figure:: talipotPythonREPL.png
  :align: center

Figure 1: Screenshot of the "Python REPL" panel in the Talipot GUI.

Talipot Python IDE
^^^^^^^^^^^^^^^^^^

A lightweight Python IDE can be accessed through the graphical interface of Talipot. It can be displayed by clicking on the "Python IDE" button located in the left part of
the Talipot interface.

.. image:: talipotPythonIDEButton.png
  :align: center

Its interface is composed of three tabs:

    * A **Scripts editor** tab (see :ref:`Figure 2<fig2>`): it allows to write scripts that can be applied to the
      graphs currently loaded in Talipot. To do so, the "main(graph)" function has to be defined in the script code
      and is used as the script entry point. The graph currently selected through the combobox located in the upper
      part of the tab is wrapped as a :class:`tlp.Graph` object and provided as parameter of the "main" function.
      The currently edited script can be launched through the control panel located in the lower part of the tab interface.
      Once started, the script execution can be stopped or paused (triggering update of Talipot visualizations) at any time.
      All modifications performed by a script on a graph can be cancelled / replayed through the Talipot undo / redo feature.

.. _fig2:
.. figure:: talipotPythonScript.png
   :align: center

   Figure 2: Screenshot of the "Scripts editor" tab in the Python IDE.

    * A **Plugins editor** tab (see :ref:`Figure 3<fig3>`): it enables to develop Talipot plugins in pure Python
      (see :ref:`Writing Talipot plugins in Python <talipotpythonplugins>`).
      These plugins are then immediately integrated in the Talipot GUI when requesting their registration
      (if their source code is valid of course). Different kinds of plugins can be developed : General Algorithms,
      Property Algorithms, Import plugins and Export plugins. When executing these plugins, standard and error output
      will be displayed in the "Message Log" panel of the Talipot GUI.

.. _fig3:
.. figure:: talipotPythonPlugin.png
   :align: center

   Figure 3: Screenshot of the "Plugins editor" tab in the Python IDE.

    * A **Modules editor** tab (see :ref:`Figure 4<fig4>`): it enables to develop Python modules that
      can be immediately imported in the current Python session. The purpose is to have a library of utility
      Python modules that can be easily imported in the developed scripts and plugins.

.. _fig4:
.. figure:: talipotPythonModule.png
   :align: center

   Figure 4: Screenshot of the "Modules editor" tab in the Python IDE.

Backup and restore your Python code with Talipot projects
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Talipot software uses a zipped project file format named TLPX to save and restore the state of a Talipot
work session in a portable way. Notably it contains the graphs that was loaded in the software serialized to
files but also the configuration of the views and components that was opened in the software.

When working with the Python IDE, the source code of the scripts, plugins and modules currently edited
are automatically saved to the current Talipot project. Nevertheless, the Talipot project must be explicitly
saved to a .tlpx file in order for the Python files to be included in the resulting archive.
When reopening the project file, the previously edited scripts, plugins and modules will still be available
in the Python IDE even if you open the file on a computer different from the one you write your code.
Python plugins will also be automatically loaded when reopening the project.

Using the autocompletion to code faster
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each Python code editor widget provides an autocompletion feature in order to ease
the development of scripts. To activate it, just hit **Ctrl + Space** and the autocompletion
list will popup. Its contents will depend on the context before the current position of the text cursor.

Python objects autocompletion
"""""""""""""""""""""""""""""

The autocompletion list feature is really useful to get the content of the dictionary
of Python objects. The list is filled by fetching the contents of a database according to the context.
That database contains the API of all standard Python modules but also the talipot ones.
That database is also updated dynamically by performing a static analysis on the source code (in particular,
that analysis tries to associate a typename to each variable in the source code).
The autocompletion will also popup immediately when hitting a dot character. If the variable before the dot
has an associated typename in the database, only the contents of its dictionary will be inserted in the list.
:ref:`Figure 5<fig5>` shows an
example of the contents of the autocompletion list when requesting it on the "graph" variable (of type :class:`tlp.Graph`)

.. _fig5:
.. figure:: autocompletion_global.png
   :align: center

   Figure 5: Using the autocompletion list to get the dictionary contents of a Python object.

Talipot special autocompletion features
"""""""""""""""""""""""""""""""""""""""

The autocompletion list is also there to ease the development of Talipot Python scripts. Some special
features have been included to ease the use of the Talipot Python API:

    * **Autocompletion for algorithms** Talipot is bundled with a lot of algorithms (plugins) that can be called through Python.
      To call an algorithm (plugin), one of the following method has to be used : :meth:`tlp.Graph.applyAlgorithm`, :meth:`tlp.Graph.applyBooleanAlgorithm`,
      :meth:`tlp.Graph.applyColorAlgorithm`, :meth:`tlp.Graph.applyDoubleAlgorithm`, :meth:`tlp.Graph.applyIntegerAlgorithm`, :meth:`tlp.Graph.applyLayoutAlgorithm`,
      :meth:`tlp.Graph.applySizeAlgorithm`, :meth:`tlp.Graph.applyStringAlgorithm`, :func:`tlp.importGraph`, :func:`tlp.exportGraph`.
      The first parameter of those method is a string containing the name of the algorithm (plugin)
      to call. When requesting the autocompletion list with the following context : *graph.apply\*Algorithm(*, it will be filled with the names of the corresponding
      algorithms (plugins). :ref:`Figure 6<fig6>` shows an example of the contents of the autocompletion list when requesting it with the following context : *graph.applyLayoutAlgorithm(*.

.. _fig6:
.. figure:: autocompletion_algos.png
   :align: center

   Figure 6: Using the autocompletion list to get the algorithm names.

    * **Autocompletion for algorithm parameters** Parameters can be passed to Talipot algorithms through a dictionary. The parameters are
      identified by their names. The autocompletion list can be used to get the names of these parameters.
      :ref:`Figure 7<fig7>` shows an example of the autocompletion list contents when requesting the parameters
      of the layout algorithm : "FM^3 (OGDF)".

.. _fig7:
.. figure:: autocompletion_algosparams.png
   :align: center

   Figure 7: Using the autocompletion list to get the algorithm parameters names.

    * **Autocompletion for string collection parameters** Some algorithms parameters are internally
      based on a :class:`tlp.StringCollection` instance. It allows to select a string from a defined set.
      The direct use of that class is now deprecated but the autocompletion list can be helpful to get the names of the
      available values that can be transmitted to the algorithm.
      :ref:`Figure 8<fig8>` shows an example of the autocompletion list contents when requesting the string collection values
      for the "Allowed Positions" parameter of the layout algorithm : "FM^3 (OGDF)".

.. _fig8:
.. figure:: autocompletion_stringcollection.png
   :align: center

   Figure 8: Using the autocompletion list to get the algorithm parameters names.

    * **Autocompletion for graph properties** Talipot stores the data associated to graph elements in objects called properties. To get a reference
      on those type of objects, you can either use specific methods (for instance : :meth:`tlp.Graph.getLayoutProperty`, :meth:`tlp.Graph.getSizeProperty`)
      that take the name of the property to retrieve as parameter or the following syntax : *graph["property name"]*. When requesting the autocompletion list
      for the following context : *graph.get*Property(* or *graph[*, the list will be filled with the names of the corresponding and existing properties.
      :ref:`Figure 9<fig9>` show an example of the contents of the autocompletion list for the following context : *graph[*.

.. _fig9:
.. figure:: autocompletion_properties.png
   :align: center

   Figure 9: Using the autocompletion list to get the graph properties names.

    * **Autocompletion list for subgraphs** Talipot allows to manipulate a large hierarchy of subgraphs. References to those subgraphs can be retrieved
      with their names through the use of the dedicated method :meth:`tlp.Graph.getSubGraph`. When requesting the autocompletion list for the
      following context : *graph.getSubGraph(*, the list will be filled with all the names of the graphs present in the hierarchy. :ref:`Figure 10<fig10>` shows
      an example of that use case.

.. _fig10:
.. figure:: autocompletion_subgraphs.png
   :align: center

   Figure 10: Using the autocompletion list to get the subgraphs names.

.. _usingBindingsInShell:

Using the bindings from the Python Interpreter
----------------------------------------------

The Talipot Python bindings can also be used through the classical Python Interpreter in an interactive shell.

The bindings modules are available on the `Python Packaging Index <https://pypi.python.org>`_.

The modules are also located within the Talipot software installation, but some setup has to be done before being able to import them.

Installing the Talipot-Python modules from the Python Packaging Index
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Talipot-Python modules can be easily obtained by using the pip tool for Windows, MacOS and Linux users.

To install the :mod:`talipot` module, issue the following command from a terminal prompt:

.. code:: shell

    $ pip install talipot-python

And you're done, you can now import the modules in your Python session.

Setting up the environment from the Talipot software installation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In order to be able to import the Talipot-Python modules bundled with the Talipot software installation,
their path must be provided to Python. In the following, <talipot_install_dir> represents the root directory of a Talipot installation.
The Talipot-Python modules are installed in the following directory according to your system :

        * Linux : **<talipot_install_dir>/lib/talipot/python** (if you compiled Talipot yourself,
          <talipot_install_dir> corresponds to the value of the CMake variable CMAKE_INSTALL_PREFIX)

        * Windows : **<talipot_install_dir>/lib/talipot/python** (if you installed Talipot from an installer,
          <talipot_install_dir> should be C:/Program Files (x86)/Talipot-X.Y/ for 32 bits Talipot and C:/Program Files/Talipot-X.Y/ for 64 bits Talipot)

        * Mac OS : **<talipot_install_dir>/Contents/lib/talipot/python** if you installed Talipot from a bundle (<talipot_install_dir> should then be /Applications/Talipot-X.Y.Z.app/) or
                   **<talipot_install_dir>/lib/talipot/python** if you compiled and installed Talipot yourself.

This path has to be added to the list of Python module search paths. To do so, you can add it in the **PYTHONPATH**
environment variable prior executing the Python interpreter or add it to the :data:`sys.path` list once you launched the interpreter.

You should now be able to import the Talipot-Python modules through the classical Python shell. Issue the following commands
at the shell prompt to perform that task::

    >>> from talipot import tlp

Customizing the Python environment
----------------------------------

It is possible to customize the Python environment the first time the :mod:`talipot` module
is imported through the use of a startup scripts hook mechanism.

For instance, that feature could be used to :

        * modify the list of Python import paths, in order to load modules not located in standard directories from then

        * load Talipot plugins not located in default plugins folders

        * add new Python functions and classes to the environment that will be available each time the talipot module is imported

When the talipot module is imported from the first time in the current Python session, the content of the following directories
will be scan for Python files (.py extension) :

        * <talipot_install_dir>/lib/talipot/python/startup

        * <home_dir>/.Talipot-X.Y/python/startup

Then, for each Python file found, its content will be read and executed in the context of the Python main module
(the file will not be imported as a Python module).


