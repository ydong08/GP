#!/usr/bin/python
# -*- coding:utf-8 -*_

import sys, os, shutil, tarfile
from git_util import *

# don't modify this
cur_dir = os.path.split(os.path.realpath(__file__))[0]
workspace_path = os.path.join(cur_dir, "workspace")
deploy_module_list = []


def replace_file_line(filepath, content):
    lines = open(filepath, 'r').readlines()
    flen = len(lines)
    for i in range(flen):
        if "deploy_games =" in lines[i]:
            lines[i] = "deploy_games = %s\n" % content
    open(filepath, 'w').writelines(lines)


def copytree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)


def get_public_repo(conf, name):
    for repo in conf.deploy_public:
        if repo.startswith(name):
            return repo
    return None

def build_game_server():
    os.chdir(workspace_path)
    # os.system("python compile.py clean")
    os.system("rm -rf %s/Output" % workspace_path)
    os.system("rm -rf CMakeLists.txt")
    os.system("python compile.py all")


def build_go_server(conf):
    git_clone(get_public_repo(conf,"gosvr"))
    os.chdir(workspace_path + "/GoServers/src/ddservers/dbserver/")
    os.putenv("GOPATH", "%s/GoServers" % workspace_path)
    os.system("go build")
    os.system("/bin/mv dbserver %s/COMMONSERVER/Output/CommonServer/Bin/MySQLServer" % workspace_path)


def build_httpsvr(conf):
    os.chdir(workspace_path)
    git_clone(get_public_repo(conf, "httpsvr"))
    os.chdir("HttpSvrUCenter")
    os.system("./compile.sh")
    output_path = "%s/Output/Http/bin" % workspace_path
    os.system("mkdir -p %s" % output_path)
    os.system("/bin/mv build/HttpSvr %s" % output_path)
    os.system("/bin/cp -r conf %s" % os.path.dirname(output_path))


def build_common_server(conf):
    os.chdir(workspace_path)
    common_path, git_url = get_git_info("common")
    if os.path.exists(common_path):
        os.chdir(common_path)
        os.system("git pull")
        os.system("git submodule update")
    else:
        git_clone(get_public_repo(conf, "common"))
        # config submodules
        os.chdir(common_path)
        os.system("git submodule init")
        git_conf_name = os.path.join(workspace_path, common_path) + "/.git/config"
        lines = open(git_conf_name, 'r').readlines()
        flen = len(lines)
        url_str = "http://%s:%s" % (conf.git_user, conf.git_password)
        for i in range(flen):
            if not url_str in lines[i]:
                if "http://" in lines[i]:
                    lines[i] = lines[i].replace("http://", "http://%s:%s@" % (conf.git_user, conf.git_password))
        open(git_conf_name, 'w').writelines(lines)
        os.system("git submodule update --init")
    os.system("python compile.py")


def build_dd_conf():
    os.chdir(workspace_path)
    git_clone("conf")


def get_server_list():
    files = os.listdir(workspace_path + "/DD_CONF/supervisor")
    print("deploy server list:")
    index = 0
    svr_list = {}
    for f in files:
        index += 1
        fn, ext = os.path.splitext(f)
        if ext == ".conf":
            print("%d - %s" % (index, fn))
            svr_list[index] = fn
    dp = raw_input("输入要部署的服务器索引，多选请用逗号分隔:[1-%d]" % index)
    if not "," in dp:
        deploy_module_list.append(svr_list[int(dp)])
    else:
        arr = dp.split(",")
        for i in arr:
            if i != "":
                deploy_module_list.append(svr_list[int(i)])
    print(deploy_module_list)


def build_all_moudle():
    print "开始编译本地所有模块"
    conf = __import__("conf")
    codepath = workspace_path
    if not os.path.exists(codepath):
        os.mkdir(codepath)

    build_dd_conf()

    os.chdir(cur_dir)
    if os.path.isfile(codepath + "/compile.py"):
        os.system("rm -rf %s" % (codepath + "/*.py"))
    shutil.copy2("compile.py", codepath + "/compile.py")
    shutil.copy2("git_util.py", codepath + "/git_util.py")
    shutil.copy2("conf.py", codepath + "/conf.py")
    os.system("chmod +x %s" % (codepath + "/compile.py"))

    if os.path.isfile(codepath + "/CMakeLists.txt"):
        os.system("rm -rf %s" % (codepath + "/CMakeLists.txt"))
    shutil.copy2("CMakeLists.txt.tmpl", codepath + "/CMakeLists.txt.tmpl")

    # git clone or pull servers
    repos = ["sdk", "share", "crypt_sdk"]
    deploy_modules = []
    for game in conf.deploy_games:
        module = game.split(":")[0]
        if module == "birdanimal":
            module = "benzbmw"
        if not game in repos:
            repos.append(game)
        if not module in deploy_modules and module not in ["common", "httpsvr", "gosvr"]:
            deploy_modules.append(module)

    # replace_file_line("conf.py", deploy_modules)
    # if os.path.isfile(codepath + "/conf.py"):
    #     os.system("rm -rf %s" % (codepath + "/conf.py"))
    # shutil.copy2("conf.py", codepath + "/conf.py")

    os.chdir(codepath)

    for rep in repos:
        git_clone(rep)

    # build servers
    build_game_server()
    build_go_server(conf)
    build_common_server(conf)
    build_httpsvr(conf)

def package_all_module():
    # change dir to parent dir
    dirname = os.path.basename(cur_dir)
    os.chdir(os.path.dirname(cur_dir))
    os.system("rm -rf %s.tar.gz" % dirname)
    sub_dirs = ["script", "sql", "*.py", "*.md"]
    workspace_dirs = ["DD_CONF", "Output", "COMMONSERVER/Output", "DD_SOFTWARE"]
    os.system("tar czf %s.tar.gz %s " % (dirname, " ".join([os.path.join(dirname, x) for x in sub_dirs]))
              + " ".join([os.path.join(dirname, "workspace", x) for x in workspace_dirs])
              + " --exclude=go1.7.1.linux-amd64.tar.gz --exclude=.git")
    os.system("/bin/mv %s.tar.gz %s/" %(dirname, dirname))