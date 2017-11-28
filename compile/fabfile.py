# -*- coding:utf-8 -*-
from __future__ import unicode_literals
import codecs
from fabric.api import *
from conf import *
from git_util import *
import json, urlparse

ftpcmd = '''ftp -n<<!
open 116.31.118.211 33321
user ddproject uxyU47741JoL
binary
cd /
lcd %s
prompt
%s %s %s
close
bye
!'''

upload_mode = "ssh" if not gateway_host else "ftp"

cur_dir = os.path.split(os.path.realpath(__file__))[0]
home_path = os.path.join(cur_dir, "workspace")
supervisor_path = "%s/DD_CONF/supervisor" % home_path
game_path = "%s/Output" % home_path
comm_path = "%s/COMMONSERVER/" % home_path
remote_path = "/home/Output"


def get_env_conf():
    global upload_mode
    env_gateway = "%s@%s" % (gateway[1], gateway[0]) if gateway[1] and gateway[2] else None
    env_game = "%s@%s" % (game_machine[1], game_machine[0])
    env_redis = "%s@%s" % (redis_machine[1], redis_machine[0])
    env_mysql = "%s@%s" % (mysql_machine[1], mysql_machine[0])
    env_password = {env_game: game_machine[2],
                    env_redis: redis_machine[2],
                    env_mysql: mysql_machine[2]
                    }
    if env_gateway:
        env_password[env_gateway] = gateway[2]
    env_roles = {"game": [env_game, ], "redis": [env_redis, ], "mysql": [env_mysql, ]}
    # print env_gateway, env_password, env_roles
    return env_gateway, env_password, env_roles


env.gateway, env.passwords, env.roledefs = get_env_conf()


def rcp(local_file, remote_file):
    put(local_file, remote_file)


def ftp(local_file, remote_file):
    filename = os.path.basename(local_file)
    dirname = os.path.dirname(local_file)
    rdirname = os.path.dirname(remote_file)
    local(ftpcmd % (dirname, 'put', filename, filename))
    run(ftpcmd % (rdirname, 'get', filename, filename))


def upload(local_file, remote_dir):
    remote_file = os.path.join(remote_dir, os.path.basename(local_file))
    print "upload:", local_file, remote_file
    if file_exists(remote_file):
        return
    if upload_mode == "ssh":
        rcp(local_file, remote_file)
    else:
        ftp(local_file, remote_file)


GameTypes = ("Alloc", "Game", "Robot", "HundredGame", "HundredRobot", "All", "Fight", "Hundred")
# GameTag,GameFolderName,supervisor_file
GameTags = (("Baccarat", "BaccaratGame", "baccarat.conf"),
            ("BenzBmw", "BaccaratGame", "benzbmw.conf"),
            ("Texas", "TexasGame", "taxas.conf"),
            ("BlackJack", "BlackJackGame", "blackjack.conf"),
            ("BullFight", "BullFight", "bull.conf"),
            ("CompareBull", "CompareBullGame", "compbull.conf"),
            ("Maajan", "DoubleMahjong", "maajan.conf"),
            ("Fish", "FishGame", "fish.conf"),
            ("Flower", "FlowerGame", "flower.conf"),
            ("Fruit", "FruitGame", "fruit.conf"),
            ("FryBull", "FryBullGame", "frybull.conf"),
            ("GuangDongMahjong", "GuangdongMahjong", "guangdong.conf"),
            ("HebeiMahjong", "HebeiMahjongGame", "hebeimahjong.conf"),
            ("HunanMahjong", "HuNanMahjongGame", ""),
            ("Land", "LandGame", "land.conf"),
            ("PaiGow", "PaiGowGame", "paigow.conf"),
            ("Showhand", "ShowhandGame", "showhand.conf"),
            ("SiChuanMahjong", "SiChuanMahjongGame", "sichuanmahjong.conf"),
            ("WuhanMahjong", "WuhanMahjongGame", "wuhan.conf"),
            ("ZhengzhouMahjong", "ZhengzhouMahjong", "zhengzhou.conf"),
            ("FishParty", "FishPartyGame", "fishparty.conf"),
            ("LandCard", "LandCardGame", "landcard.conf"),
            ("ShuangKouCard", "ShuangKouCardGame", "shuangkoucard.conf"),
            )


def find_tag(game_folder_name):
    for tag in GameTags:
        if game_folder_name == tag[1]:
            return tag
    return None


def check_category(category):
    for cate in GameTypes:
        if cate.lower() == category.lower():
            return cate
    return None


def parse_conf(conf_name, module_name):
    file_path = os.path.join(supervisor_path, conf_name)
    groups = {}
    last_group = ""
    last_program = ""
    belong_programs = []
    with open(file_path) as f:
        for line in f.readlines():
            line = line.strip()
            if line.startswith(";"):
                continue
            elif line.startswith("[group"):
                last_group = line.strip("[]").split(":")[1].strip()
                groups[last_group] = []
            elif line.startswith("programs"):
                programs = line.split("=")[1].split(",")
                groups[last_group] = [prog.strip() for prog in programs]
            elif line.startswith("[program"):
                last_program = line.strip("[]").split(":")[1].strip()
            elif line.startswith("command"):
                if os.path.basename(line.split("=")[1].split(" ")[0]) == module_name:
                    pair = []
                    for key, val in groups.iteritems():
                        if last_program in val:
                            pair.append(key)
                    pair.append(last_program)
                    belong_programs.append(pair)
    return belong_programs


def deploy(local_file, dest_path):
    filename = os.path.basename(local_file)
    upload(local_file, "~/")
    run("chmod +x ~/%s" % filename)
    run("sudo mkdir -p %s" % os.path.dirname(dest_path))
    run("sudo mv ~/%s %s" % (filename, dest_path))


def deploy_game(local_file):
    relative_dir = os.path.relpath(local_file, game_path)
    execute(deploy, local_file, os.path.join(remote_path, relative_dir))


def hallserver():
    local_file = "%s/COMMONSERVER/Output/CommonServer/bin/HallServer" % home_path
    execute(deploy, local_file, "%s/CommonServer/bin/" % remote_path)
    run("sudo supervisorctl restart hallserver:")


def httpsvr():
    local_file = "%s/Output/Http/bin/HttpSvr" % home_path
    execute(deploy, local_file, "%s/Http/bin/HttpSvr" % remote_path)
    config_httpsvr_conf()
    run("sudo supervisorctl restart httpsvr")


def redistcommon(module):
    local_file = "%s/Output/CommonServer/bin/%s" % (comm_path, module)
    execute(deploy, local_file, "%s/CommonServer/bin/" % remote_path)
    ctlname = module.replace("Server", "").lower()
    run("sudo supervisorctl restart common:%s" % ctlname)


def redist_public_server():
    redistcommon("UserServer")
    redistcommon("MySQLServer")
    redistcommon("MoneyServer")
    redistcommon("TrumptServer")
    redistcommon("RoundServer")
    hallserver()
    httpsvr()


def redist(game_folder_name, category):
    tag = find_tag(game_folder_name)
    cate = check_category(category)
    if not tag or not cate:
        print "invalid game folder name"
        return

    if cate == "All":
        modules = GameTypes[:-1]
    elif cate == "Fight":
        modules = GameTypes[:2]
    elif cate == "Hundred":
        modules = GameTypes["Alloc", "HundredGame", "HundredRobot"]
    else:
        modules = [cate]
    for mod in modules:
        module = tag[0] + mod
        subdir = mod.replace("Hundred", "") + "Server"
        local_file = "%s/%s/%s/bin/%s" % (game_path, game_folder_name, subdir, module)
        if not os.path.exists(local_file):
            print "%s not exist" % local_file
            continue
        execute(deploy_game, local_file)
        programs = parse_conf(tag[2], module)
        if not programs:
            print "can't parse %s supervisor file for %s" % (tag[2], module)
            continue
        for prog in programs:
            run("sudo supervisorctl restart %s" % ":".join(prog))


# add iptables firewall rules
def add_iptables_rules(port, source = None):
    rules = run("iptables -L -n").splitlines()
    already_in_rules = False

    for rule in rules:
        # if "Chain INPUT (policy ACCEPT)" == rule:
            # sudo("/sbin/iptables -P INPUT DROP")
            # sudo("/sbin/iptables -A INPUT -p tcp -m state --state ESTABLISH,RELATED -j ACCEPT")
            # sudo("service iptables save")
        fields = rule.split()
        if not fields:
            continue
        if fields[0] == "ACCEPT" and fields[-1] == "dpt:%s" % port:
            already_in_rules = True
    if not already_in_rules:
        source_spec = "" if not source else "-s " + source
        sudo("/sbin/iptables -A INPUT -p tcp -m state --state NEW -m tcp --dport %s %s -j ACCEPT" % (port, source_spec))
        sudo("service iptables save")


def check_conf_redis(ip):
    output = run("sudo cat /etc/hosts", quiet=True)
    lines = output.splitlines()
    no_host = True
    for line in lines:
        if "config.dd.redis" in line:
            no_host = False
    if no_host:
        run("sudo echo '%s config.dd.redis' >> /etc/hosts" % ip)


def file_exists(filepath):
    output = sudo("ls %s" % filepath, warn_only=True, quiet=True)
    stat = "No such file" not in output and "没有那个文件" not in output
    return stat


def prerare_output_path():
    if not deploy_root_path.startswith(remote_path) and not file_exists(remote_path):
        run("sudo ln -s %s /home/Output" % deploy_root_path)


def prepare_software_packages():
    os.chdir(home_path)
    local_path, url = get_git_info("software")
    remote_dir = "/home/software/"
    if not file_exists(remote_dir):
        sudo("mkdir -p -m 766 %s" % remote_dir)
    return os.path.join(home_path, local_path), remote_dir


def install_packages(packages):
    pkgs = packages.split(" ")
    output = sudo("yum list installed", quiet=True)
    for pkg in pkgs:
        if output.find(pkg + ".x86_64") == -1:
            sudo("yum -y install %s" % pkg)


def install_pyredis():
    local_path, remote_dir = prepare_software_packages()
    if not file_exists("/usr/lib/python2.6/site-packages/redis-2.10.5-py2.6.egg"):
        upload(local_path + "/python-pkg.tar.gz", remote_dir)
        with cd(remote_dir):
            sudo("tar zxf python-pkg.tar.gz", quiet=True)
            with cd(remote_dir + "/python-pkg"):
                sudo("./install_pyredis.sh", quiet=True)


def install_supervisor():
    local_path, remote_dir = prepare_software_packages()
    if not file_exists("/usr/bin/supervisord"):
        upload(local_path + "/supervisor-3.3.1.tar.gz", remote_dir)
        with cd(remote_dir):
            sudo("tar zxf supervisor-3.3.1.tar.gz", quiet=True)
            with cd("supervisor-3.3.1"):
                sudo("python setup.py install", quiet=True)
    if not file_exists("/etc/supervisord.conf"):
        sudo("echo_supervisord_conf > /etc/supervisord.conf")
        sudo("echo [include] >> /etc/supervisord.conf")
        sudo("echo 'files = /home/Output/DD_CONF/supervisor/*.conf' >> /etc/supervisord.conf")


def install_redis_server():
    install_packages("gcc gcc-c++ make")
    local_path, remote_dir = prepare_software_packages()
    if not file_exists("/usr/local/bin/redis-server"):
        if not file_exists(remote_dir + "/redis-3.2.3.tar.gz"):
            upload(local_path + "/redis-3.2.3.tar.gz", remote_dir)
            with cd(remote_dir):
                sudo("tar zxf redis-3.2.3.tar.gz", quiet=True)
        with cd(os.path.join(remote_dir, "redis-3.2.3")):
            sudo("make && make install")
            if not file_exists("/usr/bin/redis-server"):
                sudo("ln -s /usr/local/bin/redis-server /usr/bin/redis-server")
    redis_home = "/home/redis-data"
    if not file_exists(redis_home):
        sudo("mkdir -p -m 766 %s" % redis_home)
        with cd("/home/redis-data"):
            redis_conf = os.path.join(redis_home, "redis.conf")
            upload(os.path.join(cur_dir, "script/redis.conf"), redis_home)
            redis_ports = [6379, 6381, 6382, 6383, 6384, 6385, 6386, 6390]
            sudo("touch start-redis")
            sudo("chmod +x start-redis")
            sudo("echo '#!/bin/sh' >> start-redis")
            for port in redis_ports:
                sudo("mkdir -p -m 766 %s/data/%s" % (redis_home, port))
                sudo("/bin/cp  -f %s %s.conf" % (redis_conf, port))
                sudo("sed -i 's/6379/%d/g' %s.conf" % (port, port))
                sudo("echo 'redis-server /home/redis-data/%s.conf' >> start-redis" % port)
                if game_machine_host != redis_machine_host:
                    add_iptables_rules(port, game_machine_host)


def redis_exec(port, command):
    return run("redis-cli -h %s -p %s %s" % (redis_machine_host, port, command))


def add_redis_backup_crontab():
    install_packages("crontabs ftp")
    script_path = remote_path + "/backup_redis.sh"
    if not file_exists(script_path):
        upload(cur_dir + "/script/backup_redis.sh", remote_path)
        sudo("chmod +x %s" % script_path)
    output = run("sudo cat /etc/crontab")
    lines = output.splitlines()

    need_restart = False
    redis_ports = [6379, 6381, 6382, 6385, 6386]
    for port in redis_ports:
        # backup redis for every tuesday
        cron_task = "30 2 * * 2 root %s/backup_redis.sh %s %s %s %s" % \
                    (remote_path, port, backup_machine_host, backup_ftp_user, backup_ftp_password)
        no_task = True
        for line in lines:
            if not line or line.startswith("#"):
                continue
            if "backup_redis.sh %s" % port in line:
                no_task = False
        if no_task:
            run('sudo echo "%s" >> /etc/crontab' % cron_task)
            need_restart = True
    if need_restart:
        run("sudo service crond restart")


def sql_exec(command):
    return run("mysql -uroot -p%s -e \"%s\"" % (mysql_root_password, command))


def install_mysql_server():
    install_packages("gcc gcc-c++ make")
    local_path, remote_dir = prepare_software_packages()
    if not file_exists("/usr/local/mysql") or not file_exists("/usr/local/mysql/bin/mysql"):
        if not file_exists(remote_dir + "/lnmp1.3.tar.gz"):
            upload(local_path + "/lnmp1.3.tar.gz", remote_dir)
            with cd(remote_dir):
                sudo("tar zxf lnmp1.3.tar.gz", quiet=True)
        with cd(os.path.join(remote_dir, "lnmp1.3")):
            sudo("./install.sh")
    start_mysql_server()
    databases = sql_exec("show databases;")
    with lcd(cur_dir):
        if os.path.exists("sql.tar.gz"):
            local("rm -f sql.tar.gz");
        local("tar czf sql.tar.gz sql")
    if not file_exists(os.path.join(remote_dir, "sql.tar.gz")):
        rcp(cur_dir + "/sql.tar.gz", remote_dir)
        with cd(remote_dir):
            sudo("tar zxf sql.tar.gz", quiet=True)
            sql_path = os.path.join(remote_dir, "sql")
            with cd(sql_path):
                sql_files = sudo("ls").split()
                print sql_files
                for sql_filename in sql_files:
                    database_name = os.path.splitext(sql_filename)[0]
                    if database_name not in databases:
                        sql_exec("CREATE DATABASE %s;use %s;source %s;" %
                                 (database_name, database_name, os.path.join(sql_path, sql_filename)))
    # 配置权限
    users = sql_exec("use mysql;select user from user;")
    db_options = ["ucenter", "logchip", "paycenter", "statcenter", "zq_admin", "behavior", "game_route"]
    if mysql_user not in users:
        sql_exec("use mysql;create user %s IDENTIFIED by '%s';" % (mysql_user, mysql_password))
    for database in db_options:
        sql_exec("use mysql;grant all on %s.* to %s@'%%';flush privileges;" % (database, mysql_user))


def add_mysql_backup_crontab():
    install_packages("crontabs ftp")
    script_path = remote_path + "/backup_mysql.sh"
    if not file_exists(script_path):
        upload(cur_dir + "/script/backup_mysql.sh", remote_path)
        sudo("chmod +x %s" % script_path)
    # backup mysql for every tuesday
    cron_task = "30 1 * * 2 root %s/backup_mysql.sh %s %s %s %s" % \
                (remote_path, mysql_root_password, backup_machine_host, backup_ftp_user, backup_ftp_password)
    output = run("sudo cat /etc/crontab")
    lines = output.splitlines()
    no_task = True
    for line in lines:
        if not line or line.startswith("#"):
            continue
        if "backup_mysql.sh" in line:
            no_task = False
    if no_task:
        run('sudo echo "%s" >> /etc/crontab' % cron_task)
        run("sudo service crond restart")


def config_dd_conf():
    if not file_exists("%s/DD_CONF" % remote_path):
        with lcd(home_path):
            local("tar czf DD_CONF.tar.gz DD_CONF")
            upload("DD_CONF.tar.gz", remote_path)
        with cd(remote_path):
            run("sudo tar zxf DD_CONF.tar.gz --exclude=.git", quiet=True)
            run("sudo rm -f DD_CONF.tar.gz")
        hosts_file = "%s/DD_CONF/hosts" % remote_path
        if not file_exists(hosts_file):
            sudo("touch %s" % hosts_file)
            hosts_lines = open("%s/DD_CONF/hosts.tmpl" % home_path).readlines()
            for line in hosts_lines:
                fields = line.split()
                if len(fields) != 2:
                    continue
                host_map = {"mysql": mysql_host,
                            "game": game_machine_host,
                            "alloc": game_machine_host,
                            "robot": game_machine_host,
                            "server": game_machine_host,
                            "redis": redis_machine_host}
                domain_last = fields[1].split(".")[-1]
                if domain_last in host_map.keys():
                    sudo("echo '%s %s' >> %s" % (host_map[domain_last], fields[1], hosts_file))
        preconf_file = "%s/DD_CONF/preconf" % remote_path
        if not file_exists(preconf_file):
            sudo("touch %s" % preconf_file)
            sudo("echo 'mysql.ucenter.user = %s' >> %s" % (mysql_user, preconf_file))
            sudo("echo 'mysql.ucenter.passwd = %s' >> %s" % (mysql_password, preconf_file))
            sudo("echo 'mysql.logchip.user = %s' >> %s" % (mysql_user, preconf_file))
            sudo("echo 'mysql.logchip.passwd = %s' >> %s" % (mysql_password, preconf_file))


def get_extranet_ip():
    inet_addr = sudo("ifconfig|grep 'inet addr:'").splitlines()
    ip_addr = [x.lstrip("inet addr:").split()[0] for x in inet_addr]
    for addr in ip_addr:
        print addr
        if addr.startswith("10.") or addr.startswith("192.168.") or addr.startswith("172.31") or addr.startswith("127.0.0.1"):
            continue
        return addr
    return None

def config_httpsvr_conf():
    game_extranet_ip = get_extranet_ip()
    if not game_extranet_ip:
        game_extranet_ip = game_machine_host
    http_conf_path = "%s/Output/Http/conf" % home_path
    hall_conf_files = os.listdir(os.path.join(http_conf_path, "hall"))
    common_conf_files = ["ucenter.json", ]
    conf_files = [os.path.join("hall", x) for x in hall_conf_files]
    conf_files.extend(common_conf_files)
    for file_name in conf_files:
        conf_file = os.path.join(http_conf_path, file_name)
        with open(conf_file, "r") as fp:
            json_data = json.load(fp, encoding='utf-8')
            if file_name == "ucenter.json":
                mysql_keys = ["mysql_master", "mysql_slave", "mysql_routing"]
                for key in mysql_keys:
                    json_data[key]["ip"] = mysql_host
                    json_data[key]["port"] = mysql_port
                    json_data[key]["user"] = mysql_user
                    json_data[key]["passwd"] = mysql_password
                json_data["user"]["ip"] = game_machine_host
                json_data["money"]["ip"] = game_machine_host
                for item in json_data["hall"]["fish"]:
                    item["ip"] = game_extranet_ip
                    item["ip"] = game_extranet_ip
                url_keys = ["res_domain", "icon_domain"]
                for key in url_keys:
                    res = urlparse.urlparse(json_data["url"][key])
                    netloc = game_extranet_ip if not res.port else "%s:%s" % (game_machine_host, res.port)
                    json_data["url"][key] = urlparse.urlunparse(
                        [res.scheme, netloc, res.path, res.params, res.query, res.fragment])
                redis_subkeys = ["php", "param", "rank", "ote", "user", "ucnf", "mon"]
                for subkey in redis_subkeys:
                    json_data["redis"][subkey]["ip"] = mysql_host
                json_data["maintenance"]["type"] = 0
            else:
                hall_keys = ["master", "slave"]
                for key in hall_keys:
                    for item in json_data[key]["server"]:
                        item["ip"] = game_extranet_ip
        with codecs.open(conf_file, "w", "utf-8") as fp:
            fp.write(json.dumps(json_data, indent=True, ensure_ascii=False))
    os.chdir("%s/Output/Http" % home_path)
    os.system("tar czf conf.tar.gz conf")
    upload("%s/Output/Http/conf.tar.gz" % home_path, "%s/Http" % remote_path)
    with cd("%s/Http" % remote_path):
        if not file_exists("%s/Http/conf" % remote_path):
            sudo("tar zxf conf.tar.gz", quiet=True)
            sudo("rm -f conf.tar.gz")


def start_game_server_software():
    run("echo '32768 65535' > /proc/sys/net/ipv4/ip_local_port_range")
    config_dd_conf()
    output = run("sudo ps -ef|grep super")
    if "supervisord" not in output:
        run("sudo supervisord -c /etc/supervisord.conf")
    run("sudo supervisorctl reread")
    run("sudo supervisorctl update")
    with cd("/home/Output/DD_CONF"):
        run("sudo python ini2redis.py")
        run("sudo python ini2redis.py roomconfig")


def start_redis_server():
    sudo("/home/redis-data/start-redis")


def start_mysql_server():
    sudo("/etc/init.d/mysql start", warn_only=True)
