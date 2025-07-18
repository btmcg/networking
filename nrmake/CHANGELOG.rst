2.2.1 - 2020-11-08
------------------
* Use gold linker in gcc by default
* Add ``-fuse-linker-plugin`` and speed up lto with ``-flto=jobserver``


2.2.0 - 2020-10-25
------------------
* Improve search for ``Module.mk`` files.
* Change path to google-benchmark slightly. Improve build instructions.
* Reduce gcc warnings


2.1.8 - 2020-10-10
------------------
* Default to -std=c++20
* Separate definitions of ``AR`` and ``ARFLAGS`` so they can be set
  independently.
* Only set ``AR`` and ``ARFLAGS`` in compiler-specific env files.
* Allow user to override the utilities and flags used. Provide a
  centralized location to define the defaults: ``default_utilties.mk``.
* Support ``TIDYFLAGS``.
* Support ``TARFLAGS``.
* Support ``FORMATFLAGS``.
* Add ``make tidy-fix`` target that will apply fixes if possible.


2.1.7 - 2020-09-30
------------------
* Initial support for generated code using ``MODULE_GENERATED_FILES``.
  The user is expected to provide the pattern rule and/or the method by
  which the generated files are created.
* Fix bug in rwildcard where similarly named directories would match.


2.1.6 - 2020-08-29
------------------
* Fix bug in how ``.d`` files are created to ensure that the target
  inside the file contains the full path.


2.1.5 - 2020-08-29
------------------
* Add ``-Wswitch-default`` and ``-Wswitch-enum`` warnings
* Add ``help`` target


2.1.4 - 2020-08-22
------------------
* The ``clean`` target no longer removes dependency files and version
  file. The ``distclean`` target does this now.
* Dependency files are built before object files now.
* Disable all built-in rules and variables.
* Add ``MODULE_ALIAS`` to assist in dependency (``.d``) detection when
  specifying a custom ``MODULE_NAME``.


2.1.3 - 2020-07-26
------------------
* Only display ``PROJECT`` and ``ROOT_DIR`` when running ``make
  list-modules``.


2.1.2 - 2020-07-26
------------------
* Include .c and .h files in ``format`` target.
* Exclude ``benchmark-runner`` and ``test-runner`` from dist tarball.
* Define ``ROOT_DIR`` and ``PROJECT`` variables.
* Improve ``load-modules`` to avoid any variable cross-pollution between
  modules.


2.1.1 - 2020-07-23
------------------
* Go back on idea to move bins and libs to dist directories because it
  will force a re-build in the current state. Continue to just copy.


2.1.0 - 2020-07-23
------------------
* Support ``dist`` target, which will create a tarball containing the
  ``bin``, ``include``, and ``lib`` directories. The package name is
  named after the root directory with the version appended. The default
  compression is gzip, e.g., ``my_app-v1.0.0.tar.gz``
* Add ``MODULE_EXPORT_HEADERS`` and ``MODULE_EXPORT_HEADERS_PREFIX`` to
  allow the distribution of headers with the binaries and libraries. Any
  headers listed will be copied to the specified prefix path under
  ``<root>/include``.
* Replace ``version.hpp`` with ``version.h`` and remove ``constexpr``
  for better C support.
* Fix ``RUNPATH`` to use ``$ORIGIN`` so that package binaries continue
  to find libraries.
* Rename ``example_makefile`` to ``Makefile`` so that users can more
  easily symlink to it from their own project. Plus, it's not just an
  example, it is fully-functional.


2.0.3 - 2020-07-21
------------------
* Add ``*.c`` and ``*.cpp`` files in the local directory to
  ``MODULE_SOURCE_FILES`` if the variable is not defined explicitly.


2.0.2 - 2020-07-21
------------------
* Fix the ``format`` target. Again.


2.0.1 - 2020-07-20
------------------
* For the ``format`` target, check for existence of directory before
  calling find so that if a directory doesn't exist, the command doesn't
  fail.


2.0.0 - 2020-07-17
------------------
* Make fmt v7.0.0 default in ``third_party.mk``.
* [breaking change] Rename all ``LOCAL_*`` variables to ``MODULE_*``,
  which also involved changing ``LOCAL_MODULE`` to ``MODULE_NAME``.


1.0.1 - 2020-06-11
------------------
* Add ``VERSION_FILE`` as a dependency to ``tidy`` target.
* Write ``VERSION_FILE`` in a single ``sh`` command to avoid
  race-conditions.


1.0.0 - 2020-06-09
------------------
* First "official" release.
