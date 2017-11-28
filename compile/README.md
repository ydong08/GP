# dd_maintenance

##  dd游戏自动化部署脚本

### 脚本功能
    目前(2017-1-19)已实现自动clone代码，自动编译指定的服务器（包括goserver），自动打包程序并上传到目标服务器，目标
    服务器上自动解压，运行功能

**注意：实际部署过程，应该提前规划好各游戏以及公共服务器要部署的服务器ip地址

### 使用方法
    1、打包（本地）机器环境
        操作系统: Cent OS 6.5
    2、目标服务器环境要求
        操作系统: Cent OS 6.5
    3、操作流程

    一）本地打包
        1)运行deploy.py脚本，会生成conf.py配置文件
        2)修改conf.py，指定git_user（clone代码所需git账号），git_password（git密码）
        3)修改conf.py，添加deploy_games
        4）运行python deploy.py build，将会自动下载所需编译的git仓库，编译，打包
        打包文件位于本文件夹下，为dd_maintenance.tar.gz

    二）上传打包文件到目标服务器
        1)上传，解压文件

    三）游戏部署
        1)修改conf.py
             指定deploy_root_path，空间比较大的磁盘目录
             指定redis_machine_server,mysql_machine_server,game_machine_server相关参数
        2)运行脚本
            运行python deploy.py redis，只部署redis服务器
            运行python deploy.py mysql，只部署mysql服务器
            运行python deploy.py game，只部署游戏相关服务器
            运行python deploy.py all，部署全部服务器
    4、部署过程可能出现问题的地方
        1)如果fabric安装成功，运行失败，则清除python Crypto模块，从新安装
        rm -rf /usr/lib64/python2.6/site-packages/Crypto
    
### 后续待改进内容
    1、使用过程中碰到的bug修复
    2、其他需求或者改进可在此列出。