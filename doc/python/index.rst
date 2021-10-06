.. highlight:: python

Welcome to Talipot Python documentation!
========================================

Talipot is an information visualization framework written in C++ dedicated to the analysis and visualization of graphs.
Talipot Python is a set of modules that exposes to Python almost all the content of the Talipot C++ API. The bindings
has been developed with the SIP_ tool from Riverbank_. The main
features provided by the bindings are listed below :

* creation and manipulation of graphs
* storage of data on graph elements (float, integer, boolean, color, size, coordinate, list, ...)
* application of algorithms of different types on graphs (layout, metric, clustering, ...)
* the ability to write Talipot plugins in pure Python

The bindings can be used inside the Talipot software GUI in order to run scripts on the current visualized graph.
The bindings can also be used outside Talipot through the classical Python interpreter.

Contents
========

.. toctree::
   :maxdepth: 3

   gettingstarted
   apichanges
   manual
   pythonplugins
   talipotpluginsdocumentation
   talipotreference
   talipotpluginsreference

Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

.. _SIP: http://www.riverbankcomputing.co.uk/software/sip/intro

.. _Riverbank: http://www.riverbankcomputing.co.uk/
