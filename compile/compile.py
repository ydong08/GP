#!/usr/bin/python
# -*- coding:utf-8 -*-

import sys, os, hashlib, datetime, platform, time, pickle, shutil, conf, git_util

from DD_SDK.blibs import *
from DD_SDK.blibs.util import *


def find_cmake_files_recursive(dirname):
    result = []
    if "build" in dirname or "Output" in dirname or ".git" in dirname:
        return result
    print("scan " + dirname)
    time.sleep(0.01)
    files = os.listdir(dirname)
    for f in files:
        subpath = os.path.join(dirname, f)
        if os.path.isfile(subpath):
            filename = os.path.basename(subpath)
            if filename == "CMakeLists.txt":
                result.append(os.path.abspath(subpath))
        else:
            res = find_cmake_files_recursive(subpath)
            if res:
                result.extend(res)
    return result


def parse_cmake_file(path):
    result = []
    if not os.path.isfile(path):
        return result
    for line in open(path):
        if "option" in line and line[0] != '#':
            opt_start = line.find("(")
            opt_end = line.find(" ")
            assert (opt_start != -1 and opt_end != -1)
            opt = line[opt_start + 1:opt_end]
            # print(opt)
            if "COMMON" not in line:
                key = opt[len("BUILD_"):]
                result.append(key)
                # print(result)
            else:
                key = opt[len("BUILD_"):]
                key = key[:key.rfind("_SERVER")]
                result.append(key)
                # print(key)
    return result


def print_help():
    print("please enter build name")


def build_tree(opts, fields):
    if len(fields) > 0:
        if not opts.has_key(fields[0]):
            opts[fields[0]] = {}
        build_tree(opts[fields[0]], fields[1:])


def print_tree(opts, tab):
    for k in opts:
        print tab + "\t", k
        print_tree(opts[k], tab + "\t")


def print_tip(build_opt):
    gameopts = {}
    for opt in build_opt:
        fields = opt.split('_')
        build_tree(gameopts, fields)

    print "./compile.py"
    print_tree(gameopts, "")


def add_game(fo, gamename):
    if "benzbmw" == gamename:
        fo.writelines("add_subdirectory(${CMAKE_HOME_DIRECTORY}/BenzBmw/BenzBmw/)\n")
        fo.writelines("add_subdirectory(${CMAKE_HOME_DIRECTORY}/BenzBmw/BirdAnimal/)\n")
        fo.writelines("add_subdirectory(${CMAKE_HOME_DIRECTORY}/BenzBmw/TigerGame/)\n")
    elif "bull" == gamename:
        fo.writelines("add_subdirectory(${CMAKE_HOME_DIRECTORY}/BullFight/)\n")
    else:
        line_text = "add_subdirectory(${CMAKE_HOME_DIRECTORY}/" + git_util.game_git_path[gamename] + "/)\n"
        fo.writelines(line_text)


if __name__ == "__main__":
    argv_cnt = len(sys.argv)
    if argv_cnt <= 1:
        print_help()
        exit()

    cur_dir = os.getcwd()

    # generate_version("./DD_SDK/include/comm/game_version.h.tmpl")
    # print(cur_dir)
    build_dir = "build"
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    os.chdir(build_dir)

    cmakefile = cur_dir + "/" + build_dir + "/cmake.txt"

    argvupper = [arg.upper() for arg in sys.argv[1:]]
    argv1 = sys.argv[1].upper()
    if argv1 == "CLEAN":
        if os.path.exists(cur_dir + "/" + build_dir):
            print("remove build dir now...")
            os.chdir(cur_dir)
            shutil.rmtree(cur_dir + "/" + build_dir)
        exit()
    elif argv1 == "REFRESH":
        print("remove temp file now...")
        if os.path.isfile(cmakefile):
            os.remove(cmakefile)
        exit()

    cmakelist = []
    if os.path.isfile(cmakefile):
        f = open(cmakefile, 'r')
        cmakelist = pickle.load(f)
    else:
        # print(cmakefile)
        f = open(cmakefile, 'wb')
        cmakelist = find_cmake_files_recursive("../")
        pickle.dump(cmakelist, f)
    # print(cmakelist)
    os.chdir(cur_dir)
    if os.path.exists("CMakeLists.txt"):
        os.remove("CMakeLists.txt")
    shutil.copy2("CMakeLists.txt.tmpl", "CMakeLists.txt")

    fo = open("CMakeLists.txt", 'a')
    fo.seek(0, 2)
    for rep in [x.split(":")[0] for x in conf.deploy_games]:
        add_game(fo, rep)
    fo.close()
    cmd = batch_command(cur_dir + "/" + build_dir)

    build_opt = []
    for cmakepath in cmakelist:
        build_opt.extend(parse_cmake_file(cmakepath))

    cmake_opt = "-DCMAKE_BUILD_TYPE=Debug "
    have_build = False
    for opt in build_opt:
        # print(opt)
        if opt.startswith("COMMON"):
            opt = opt + "_SERVER"
        cmake_opt = cmake_opt + "-DBUILD_" + opt
        if argv_cnt == 2:
            if argv1 == "ALL" or opt.upper().startswith(argv1):
                cmake_opt = cmake_opt + "=TRUE "
                have_build = True
            else:
                cmake_opt = cmake_opt + "=FALSE "
        elif argv_cnt > 2:
            spec_opt = '_'.join(argvupper)
            if opt.upper().startswith(spec_opt):
                cmake_opt = cmake_opt + "=TRUE "
                have_build = True
            else:
                cmake_opt = cmake_opt + "=FALSE "
        else:
            cmake_opt = cmake_opt + "=FALSE "

    if not have_build:
        print "please input correct module name"
        print_tip(build_opt)
        exit()

    # print(cmake_opt)
    os.chdir(build_dir)
    cmd.add_execmd("cd " + cur_dir + "/" + build_dir)
    cmd.add_execmd("build_option=\"" + cmake_opt + "\"")
    cmd.add_execmd_with_error_exit("cmake $build_option ../")
    cmd.add_execmd_with_error_exit("make")

    cmd.execute()
