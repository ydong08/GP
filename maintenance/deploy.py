# -*- coding:utf-8 -*-

import sys, os, shutil
from git_util import *
from build import *

def local_install_package(package_names):
    packages = package_names.split(" ")
    output = os.popen("yum list installed").read()
    for pkg in packages:
        if output.find(pkg + ".x86_64") == -1:
            os.system("yum -y install %s" % pkg)

def local_install_fabric():
    local_install_package("make gcc gcc-c++ python-devel")
    software_path = os.path.join(workspace_path, "DD_SOFTWARE")
    os.chdir(software_path)
    if not os.path.exists("/usr/bin/fab"):
        os.system("tar zxf python-pkg.tar.gz")
        # 系统自带的Crypto模块版本不匹配，需删除重装
        if os.path.exists("/usr/lib64/python2.6/site-packages/Crypto"):
            os.system("rm -rf /usr/lib64/python2.6/site-packages/Crypto")
        os.chdir(os.path.join(software_path, "python-pkg"))
        os.system("chmod +x *.sh")
        os.system("./install_fabric.sh")


def local_install_go():
    software_path = os.path.join(workspace_path, "DD_SOFTWARE")
    os.chdir(software_path)
    if not os.path.exists("/usr/local/go"):
        os.chdir(software_path)
        os.system("tar zxf go1.7.1.linux-amd64.tar.gz")
        os.system("sudo mv %s/go /usr/local/" % software_path)
        os.system("echo 'export PATH=$PATH:/usr/local/go/bin' >> /etc/profile")
    # set GOROOT
    os.putenv("PATH", "%s:/usr/local/go/bin" % os.getenv("PATH"))
    os.putenv("GOROOT", "/usr/local/go")


def build_local_env():
    print "正在安装本地工作环境"
    local_install_package("openssh-clients tar git cmake make gcc gcc-c++ python-devel libidn-devel zlib-devel")
    os.chdir(workspace_path)
    git_clone("software")
    local_install_fabric()
    local_install_go()


def fab_exec(role, command):
    os.system("fab -f fabfile.py -R %s %s" % (role, command))


def deploy_redis_server():
    fab_exec("redis", "install_redis_server")
    fab_exec("redis", "add_redis_backup_crontab")
    fab_exec("redis", "start_redis_server")


def deploy_mysql_server():
    fab_exec("mysql", "install_mysql_server")
    fab_exec("mysql", "add_mysql_backup_crontab")
    fab_exec("mysql", "start_mysql_server")


def deploy_game_server():
    conf = __import__("conf")
    errmsg = None
    if not os.path.exists(conf.deploy_root_path):
        errmsg = "请指定正确的deploy_root_path目录，请确保文件夹存在"
    if conf.deploy_root_path.startswith("/home/Output"):
        errmsg = "请指定正确的deploy_root_path目录，它不能为/home/Output或其子目录"
    if errmsg:
        print errmsg
        exit(-1)
    fab_exec("game", "prerare_output_path")
    fab_exec("game", "install_pyredis")
    fab_exec("game", "install_supervisor")
    # config /etc/hosts config.dd.redis
    fab_exec("game", "check_conf_redis:%s" % conf.redis_machine_host)
    listen_ports = [20000, 9011]
    for port in listen_ports:
        fab_exec("game", "add_iptables_rules:%s" % port)
    deploy_game_and_start(conf)


def deploy_game_and_start(conf):
    print "开始部署游戏并启动"
    fab_exec("game", "start_game_server_software")
    fab_exec("game", "redist_public_server")

    module_folder_map = {"bull": "BullFight", "taxas": "TexasGame"}
    for module in [x.split(":")[0] for x in conf.deploy_games]:
        game_folder_name = game_git_path[module]
        if module.split("_")[0] in module_folder_map.keys():
            game_folder_name = module_folder_map[module]
        cate = "Hundred" if module.endswith("hundred") else "Fight"
        fab_exec("game", "redist:%s,%s" % (game_folder_name, cate))


if __name__ == "__main__":
    target = sys.argv[1].lower() if len(sys.argv) >= 2 else "all"
    if target not in ["build", "redis", "mysql", "game", "all"]:
        print '''python deploy.py [build|redis|mysql|game|all]
        build: 本地编译及打包
        redis: 只部署redis服务
        mysql: 只部署mysql服务
        game: 只部署game相关服务
        all: 部署所有服务'''
        sys.exit(1)
    if not os.path.isfile(cur_dir + "/conf.py"):
        shutil.copy2("conf.tmpl", "conf.py")
        print("conf.py 文件产生了.\n请编辑相关参数，并再次运行python depoly.py")
        sys.exit(1)

    if target == "build":
        if not os.path.exists(workspace_path):
            os.system("mkdir -p %s" % workspace_path)
        os.chdir(workspace_path)
        build_local_env()
        build_all_moudle()
        package_all_module()
    else:
        local_install_fabric()
        if target in ["redis", "all"]:
            deploy_redis_server()
        if target in ["mysql", "all"]:
            deploy_mysql_server()
        if target in ["game", "all"]:
            deploy_game_server()

    # remove tmp file
    os.chdir(cur_dir)
    os.system("rm -rf *.pyc")
