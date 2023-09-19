""" The CLI entry point for exporting projects from the mbed tools to any of the
supported IDEs or project structures.
"""
import sys
from os.path import join, abspath, dirname, exists, basename
ROOT = abspath(join(dirname(__file__), ".."))
sys.path.insert(0, ROOT)

from shutil import move, rmtree
from argparse import ArgumentParser
from os.path import normpath, realpath
import os

from exports.paths import EXPORT_DIR, MBED_HAL, MBED_LIBRARIES, MBED_TARGETS_PATH
from exports.settings import BUILD_DIR
from exports.export import EXPORTERS, mcu_ide_matrix, mcu_ide_list, export_project, get_exporter_toolchain
from exports.tests import TESTS, TEST_MAP
from exports.tests import test_known, test_name_known, Test
from exports.targets import TARGET_NAMES
from exports.utils import argparse_filestring_type, argparse_profile_filestring_type, argparse_many, args_error
from exports.utils import argparse_force_lowercase_type
from exports.utils import argparse_force_uppercase_type
from exports.utils import print_large_string
from exports.options import extract_profile, list_profiles, extract_mcus

def setup_project(ide, target, program=None, source_dir=None, build=None, export_path=None):
    """Generate a name, if not provided, and find dependencies

    Positional arguments:
    ide - IDE or project structure that will soon be exported to
    target - MCU that the project will build for

    Keyword arguments:
    program - the index of a test program
    source_dir - the directory, or directories that contain all of the sources
    build - a directory that will contain the result of the export
    """
    # Some libraries have extra macros (called by exporter symbols) to we need
    # to pass them to maintain compilation macros integrity between compiled
    # library and header files we might use with it
    if source_dir:
        # --source is used to generate IDE files to toolchain directly
        # in the source tree and doesn't generate zip file
        project_dir = export_path or source_dir[0]
        if program:
            project_name = TESTS[program]
        else:
            project_name = basename(normpath(realpath(source_dir[0])))
        src_paths = source_dir
        lib_paths = None
    else:
        test = Test(program)
        if not build:
            # Substitute the mbed library builds with their sources
            if MBED_LIBRARIES in test.dependencies:
                test.dependencies.remove(MBED_LIBRARIES)
                test.dependencies.append(MBED_HAL)
                test.dependencies.append(MBED_TARGETS_PATH)


        src_paths = [test.source_dir]
        lib_paths = test.dependencies
        project_name = "_".join([test.id, ide, target])
        project_dir = join(EXPORT_DIR, project_name)

    return project_dir, project_name, src_paths, lib_paths

def cp_new_prj(prj):
    base_dir = "rabbit/"
    prj_patten = "prj_demo_"
    prj_base = base_dir + "prj/examples/"

    tmp_str = "project"
    pro_list = os.listdir(prj_base)
    if not prj:
        prj = pro_list[0]
        tmp_str = "the first " + tmp_str
        print "Project empty!",

    prj_path = prj_base + prj
    new_prj_path = base_dir + prj_patten + prj

    cmd = "rm -rf " + base_dir + prj_patten + "*"
    os.popen(cmd)

    if not os.path.exists(prj_path):
        print "\nSupported projects are the followings:"
        for p in pro_list:
            print p
        sys.exit()
    else:
        cmd = "cp -ar " + prj_path + " " + new_prj_path
        os.popen(cmd)
        print "Set", tmp_str, "[", prj, "] to be the demo project successfully!\n"

    #delete the same project
    #if os.path.exists(new_prj_path):
    #    cmd = "rm -rf " + new_prj_path
    #    os.popen(cmd)


def export(target, ide, build=None, src=None, macros=None, project_id=None,
           zip_proj=False, build_profile=None, export_path=None, silent=False,
           app_config=None):
    """Do an export of a project.

    Positional arguments:
    target - MCU that the project will compile for
    ide - the IDE or project structure to export to

    Keyword arguments:
    build - to use the compiled mbed libraries or not
    src - directory or directories that contain the source to export
    macros - extra macros to add to the project
    project_id - the name of the project
    clean - start from a clean state before exporting
    zip_proj - create a zip file or not

    Returns an object of type Exporter (tools/exports/exporters.py)
    """
    project_dir, name, src, lib = setup_project(ide, target, program=project_id,
                                                source_dir=src, build=build, export_path=export_path)

    zip_name = name+".zip" if zip_proj else None

    return export_project(src, project_dir, target, ide, name=name,
                          macros=macros, libraries_paths=lib, zip_proj=zip_name,
                          build_profile=build_profile, silent=silent,
                          app_config=app_config)


def main():
    """Entry point"""
    # Parse Options
    parser = ArgumentParser()

    targetnames = TARGET_NAMES
    targetnames.sort()
    toolchainlist = EXPORTERS.keys()
    toolchainlist.sort()

    parser.add_argument("-m", "--mcu",
                        metavar="MCU",
                        type=str.upper,
                        help="generate project for the given MCU ({})".format(
                            ', '.join(targetnames)))

    parser.add_argument("-i",
                        dest="ide",
                        type=argparse_force_lowercase_type(
                            toolchainlist, "toolchain"),
                        help="The target IDE: %s"% str(toolchainlist))

    parser.add_argument("-c", "--clean",
                        action="store_true",
                        default=False,
                        help="clean the export directory")

    #terrylee
    parser.add_argument("-prj",
                        default=False,
                        help="take this project demo as the main entry.(Default is the first project demo)")

    group = parser.add_mutually_exclusive_group(required=False)
    group.add_argument(
        "-p",
        type=test_known,
        dest="program",
        help="The index of the desired test program: [0-%s]"% (len(TESTS)-1))

    group.add_argument("-n",
                       type=test_name_known,
                       dest="program",
                       help="The name of the desired test program")

    parser.add_argument("-b",
                      dest="build",
                      default=False,
                      action="store_true",
                      help="use the mbed library build, instead of the sources")

    group.add_argument("-L", "--list-tests",
                       action="store_true",
                       dest="list_tests",
                       default=False,
                       help="list available programs in order and exit")

    group.add_argument("-S", "--list-matrix",
                       dest="supported_ides",
                       default=False,
                       const="matrix",
                       choices=["matrix", "ides"],
                       nargs="?",
                       help="displays supported matrix of MCUs and IDEs")

    parser.add_argument("-E",
                        action="store_true",
                        dest="supported_ides_html",
                        default=False,
                        help="writes tools/export/README.md")

    parser.add_argument("--source",
                        action="append",
                        type=argparse_filestring_type,
                        dest="source_dir",
                        default=[],
                        help="The source (input) directory")

    parser.add_argument("-D",
                        action="append",
                        dest="macros",
                        help="Add a macro definition")

    parser.add_argument("--profile", dest="profile", action="append",
                        type=argparse_profile_filestring_type,
                        help="Build profile to use. Can be either path to json" \
                        "file or one of the default one ({})".format(", ".join(list_profiles())),
                        default=[])

    parser.add_argument("--update-packs",
                        dest="update_packs",
                        action="store_true",
                        default=False)
    parser.add_argument("--app-config",
                        dest="app_config",
                        default=None)

    options = parser.parse_args()

    # Print available tests in order and exit
    if options.list_tests is True:
        print '\n'.join([str(test) for test in  sorted(TEST_MAP.values())])
        sys.exit()

    # Only prints matrix of supported IDEs
    if options.supported_ides:
        if options.supported_ides == "matrix":
            print_large_string(mcu_ide_matrix())
        elif options.supported_ides == "ides":
            print mcu_ide_list()
        exit(0)

    # Only prints matrix of supported IDEs
    if options.supported_ides_html:
        html = mcu_ide_matrix(verbose_html=True)
        try:
            with open("./export/README.md", "w") as readme:
                readme.write("Exporter IDE/Platform Support\n")
                readme.write("-----------------------------------\n")
                readme.write("\n")
                readme.write(html)
        except IOError as exc:
            print "I/O error({0}): {1}".format(exc.errno, exc.strerror)
        except:
            print "Unexpected error:", sys.exc_info()[0]
            raise
        exit(0)

    if options.update_packs:
        from exports.arm_pack_manager import Cache
        cache = Cache(True, True)
        cache.cache_descriptors()

    #terrylee
    cp_new_prj(options.prj)

    # Target
    if not options.mcu:
        args_error(parser, "argument -m/--mcu is required")

    # Toolchain
    if not options.ide:
        args_error(parser, "argument -i is required")

    # Clean Export Directory
    if options.clean:
        if exists(EXPORT_DIR):
            rmtree(EXPORT_DIR)

    zip_proj = not bool(options.source_dir)

    if (options.program is None) and (not options.source_dir):
        args_error(parser, "one of -p, -n, or --source is required")
    exporter, toolchain_name = get_exporter_toolchain(options.ide)
    mcu = extract_mcus(parser, options)[0]
    if not exporter.is_target_supported(mcu):
        args_error(parser, "%s not supported by %s"%(mcu,options.ide))
    profile = extract_profile(parser, options, toolchain_name, fallback="debug")
    if options.clean:
        rmtree(BUILD_DIR)
    export(mcu, options.ide, build=options.build,
           src=options.source_dir, macros=options.macros,
           project_id=options.program, zip_proj=zip_proj,
           build_profile=profile, app_config=options.app_config)


if __name__ == "__main__":
    main()
