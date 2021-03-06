TOra - Toolkit for Oracle
=========================

TOra - An Oracle Toolkit for DBA's and developers

Shared/mixed copyright is held throughout files in this product

Portions Copyright (C) 2000-2001 Underscore AB
Portions Copyright (C) 2003-2005 Quest Software, Inc.
Portions Copyright (C) 2004-2013 Numerous Other Contributors

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;  only version 2 of
the License is valid for this program.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program as the file COPYING.txt; if not, please see
http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt.

     As a special exception, you have permission to link this program
     with the Oracle Client libraries and distribute executables, as long
     as you follow the requirements of the GNU GPL in regard to all of the
     software in the executable aside from Oracle client libraries.

All trademarks belong to their respective owners.

For information about license see the COPYING file. TOra is 
distributed under the GPL which basically means that any derivatory 
work must be released under the GPL license as well.

TOra is supported for running with an Oracle 8.1.7 or newer
client installation. It has been verified to work with Oracle 10g and 11g.

WINDOWS / ORACLE NOTICE: The windows build of tora includes a stub Oracle DLL to
enable the product to work with other database engines even if oracle is not 
installed. If you have Oracle client installed, and want oracle support in TOra to
function, remove the oci.dll file from the TOra installation directory after 
installing it.

In addition, TOra also supports postgres and mysql if your Qt library
is compiled with that support.

Toolkit for Oracle is a program for database developers and administrators. The
features that are available so far is (As of version 1.2):

* Handles multiple connections
* Support Oracle & MySQL
* Advanced SQL Worksheet
	* Explain plan
	* PL/SQL auto indentation
	* Statement statistics
	* Error location indication
	* SQL syntax highlighting
	* Code completion
	* Visualization of result
	* PL/SQL block parsing
	* Statement statistics comparison
* Schema browser
	* Table & view editing
	* References & dependencies
	* Reverse engeneering of objects
	* Tab & tree based browsing
	* Object & data filtering
* PL/SQL Editor/Debugger
	* Breakpoints
	* Watches
	* Line stepping
	* SQL Output viewing
	* Structure tree parsing
* Server tuning
	* Server overview
	* Tuning charts
	* Wait state analyzer
	* I/O by tablespace & file
	* Performance indicators
	* Server statistics
	* Parameter editor (P-file editor)
* Security manager
* Storage manager with object & extent viewer

* Session manager
* Rollback manager with snapshot too old detection
* SGA and long operations trace
* Current session information

* PL/SQL profiler
* Explain plan browser
* Statistics manager
* DBMS alert tool
* Invalid object browser
* SQL Output viewer
* Database/schema comparison and search
* Extract schema objects to SQL script

* Easily extendable
* Possible to add support for new or older Oracle versions without programming.
* SQL template help
* Full UNICODE support
* Printing of any list, text or chart

For installation instructions se the file INSTALL.

TOra is developed by a community of Open Source developers. The original 
(pre 1.3.15) development was done by Henrik Johnson of Quest Software, Inc.

The homepage for the TOra project is http://tora.sourceforge.net. If you 
encounter problems you can find both mailinglists and bugtracking tools 
from this page.
