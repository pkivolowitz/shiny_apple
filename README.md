# Shiny Apple

This command line tool will recursively search from a specified folder
looking for the leavings of compilers on the Apple platform. It cleans
and shines your Apple development environment.

Specifically the following:

* .dSYM directories which are automatically made to hold debugging
information. These directories are safely deleted when development is
not taking place. The -f option is followed by the directory at which to
start. If -f is not given, the search begins with the current directory.

* a.out files can optionally be removed. These are the default
  executables made during a typical command line based build process.
  If -a is given, any a.out files will be removed.

* .d files can be safely removed. These are dependency files made during
  some build processes. If the -d option is given, files ending in .d
  are deleted.

* .o files can be safely deleted. These are intermediate files created
  duing a build. If the -o option is given, files ending in .o are
  deleted.

The -s option, if given, causes the program to abort upon encountering
an error such as not being able to delete a target due to permissions.
If -s is not given, the program will continue despite any errors.

The -A options is equivalent to specifying -o -d -a.

By default, the program prints a statement about each file or directory
it deletes. If the -q option is given, this printing is supressed.
