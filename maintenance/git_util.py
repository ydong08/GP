# -*- coding:utf-8 -*-
import os

# name of repository
game_git_path = {
    "conf": "DD_CONF",
    "sdk": "DD_SDK",
    "crypt_sdk": "DDCrypt_SDK",
    "share": "DD_SHARE",
    "httpsvr": "HttpSvrUCenter",
    "gosvr": "GoServers",
    "common": "COMMONSERVER",
    "software": "DD_SOFTWARE",
    # follow are games
    "land": "LandGame",
    "bull": "DD_BullFight",
    "bull_hundred": "DD_BullFight",
    "zhengzhou": "ZhengzhouMahjong",
    "sichuan": "SichuanMahjongGame",
    "hunan": "HuNanMahjongGame",
    "hebei": "HebeiMahjongGame",
    "double": "DoubleMahjong",
    "benzbmw": "BenzBmw",
    "paigow": "PaiGowGame",
    "comparebull": "CompareBullGame",
    "frybull": "FryBullGame",
    "showhand": "ShowhandGame",
    "showhand_hundred": "ShowhandGame",
    "fish": "FishGame",
    "baccarat": "BaccaratGame",
    "baccarat_hundred": "BaccaratGame",
    "wuhan": "WuhanMahjongGame",
    "guangdong": "GuangdongMahjong",
    "flower": "FlowerGame",
    "flower_hundred": "FlowerGame",
    "fruit": "FruitGame",
    "fishparty": "FishPartyGame",
    "chinesechess": "ChineseChess",
    "guobiao": "GuobiaoMahjongGame",
    "taxas": "ShowhandGame",
    "taxas_hundred": "ShowhandGame",
    "blackjack": "BlackJackGame",
    "landcard": "LandCardGame",
    "shuangkoucard": "ShuangKouCardGame"
}


def get_git_info(repo_name):
    name = repo_name.split(":")[0]
    if not game_git_path.has_key(name):
        print("not valid in game git path: %s" % name)
        exit(1)
    if name == "share":
        local_path = "Common"
    elif name == "bull":
        local_path = "BullFight"
    else:
        local_path = game_git_path[name]
    conf = __import__("conf")
    url = conf.git_url % (conf.git_user, conf.git_password, game_git_path[name])
    return local_path, url


def git_clone(repo_name):
    repo_list = repo_name.split(":")
    branch = "master" if len(repo_list) == 1 else repo_list[1]
    local_path, url = get_git_info(repo_name)
    if os.path.exists(local_path):
        curdir = os.getcwd()
        update_path = curdir + "/" + local_path
        os.chdir(update_path)
        os.system("git pull")
        os.chdir(curdir)
    else:
        os.system("git clone %s %s" % (url, local_path))

    if branch != "master":
        curdir = os.getcwd()
        os.chdir(local_path)
        os.system("git checkout %s" % branch)
        os.system("git pull origin %s" % branch)
        os.chdir(curdir)
