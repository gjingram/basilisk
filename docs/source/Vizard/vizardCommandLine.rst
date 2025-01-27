.. _vizardCommandLine:

Command Line Launching
======================
This page contains a series of terminal commands to open Vizard from the command line on macOS
and provide optional arguments.  Adjust the command line ``open`` command and file name according
to your platform and how you named the Vizard application.  The examples below it is assumed the
application is called ``Vizard.app`` and it is installed in the syste ``/Applications`` folder.

To launch an instance of the `Vizard <vizard>`_ application from the command line::

	open /Applications/Vizard.app

The `Vizard <vizard>`_ application supports the following command line arguments:

#. To launch `Vizard <vizard>`_ application in opNav mode, use the ``-opNav`` argument followed
   by the TCP address `Vizard <vizard>`_ should connect to::

	open /Applications/Vizard.app --args -opNav tcp://xxx.xxx.x.xx.xxxxx

#. To launch `Vizard <vizard>`_ application in opNav mode without displaying anything to the screen,
   use the argument ``-batchmode`` with the ``-opNav`` argument::

	open /Applications/Vizard.app --args -batchmode -opNav tcp://xxx.xxx.x.xx.xxxxx

#. To launch `Vizard <vizard>`_ application with a livestreaming connection to BlackLion,
   use the ``-blackLion`` argument followed by the TCP address `Vizard <vizard>`_ should connect to::

	open /Applications/Vizard.app --args -blackLion tcp://xxx.xxx.x.xx:xxxxx

#. To launch `Vizard <vizard>`_ application with a livestreaming connection to DirectComm,
   use the ``-directComm`` argument followed by the TCP address `Vizard <vizard>`_ should connect to::

	open /Applications/Vizard.app --args -directComm  tcp://xxx.xxx.x.xx:xxxxx

#. To save a copy of all vizMessages passed to `Vizard <vizard>`_ during the run,
   use the ``-saveMsgFile`` argument. Optionally, this argument can be followed by the desired
   file name that `Vizard <vizard>`_ will name the saved message file on exit::

	open /Applications/Vizard.app --args -saveMsgFile filenameToUse
