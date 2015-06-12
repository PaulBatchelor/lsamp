LSamp
=====

LSamp is a Lightweight Sampler format. It intends to be a simple and easy
to use monolithic sampler format. 

Features:
- Supports a wide range of file formats
- key/value interface for storing samples
- sane(ish) binary format (sqlite3 + blobs)
- small codebase
- simple API
- easily embeddable into larger projects
- CLI utility for adding files

Compile + Install:
------------------

make
sudo make install

CLI Usage:
----------

To create a new lsamp file:

lsamp init foo.smp
   
To add file "foo.wav" to "foo.smp" with the keyword "bar":

lsamp add foo.smp bar foo.wav

Too remove keyword "bar" from "foo.smp"

lsamp remove foo.smp bar
