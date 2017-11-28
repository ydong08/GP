IF (__DDCRYPTO__)
project (HttpCSvr)
ELSE ()
project (HttpSvr)
ENDIF ()
cmake_minimum_required(VERSION 2.8.6)
SET(CMAKE_CXX_FLAGS "-std=c++0x -g")

macro(source_group_by_dir source_files strip_head)
    if(CMAKE_SYSTEM MATCHES "Windows" OR CMAKE_SYSTEM MATCHES "Darwin")
        set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
        foreach(sgbd_file ${${source_files}})
            string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
            string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
            string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
            string(REPLACE "/" "\\" sgbd_group_name ${sgbd_group_name})
            if(sgbd_nogroup)
                set(sgbd_group_name "\\")
            else()
                set(strip_len 0)
                string(LENGTH ${strip_head} strip_len)
                set(group_len 0)
                string(LENGTH ${sgbd_group_name}, group_len)
                string(COMPARE EQUAL ${strip_head} "./" equal_cur)
                if (NOT (${strip_head} STREQUAL "./"))
                    string(SUBSTRING ${sgbd_group_name} ${strip_len} ${group_len} sgbd_group_name)
                endif()
                #message(${sgbd_group_name})
            endif(sgbd_nogroup)
            source_group(${sgbd_group_name} FILES ${sgbd_file})
        endforeach(sgbd_file)
    endif(CMAKE_SYSTEM MATCHES "Windows" OR CMAKE_SYSTEM MATCHES "Darwin")
endmacro(source_group_by_dir)


IF (__DDCRYPTO__)
set(proj_name HttpCSvr)
ELSE ()
set(proj_name HttpSvr)
ENDIF ()

set(src_path
	"./"
)


#list all files
file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_hppeaders *.hpp)
file(GLOB_RECURSE handle_cpps http_handle/*.cpp)
file(GLOB_RECURSE comm_cpps comm/*.cpp)
file(GLOB main_cpps *.cpp)
set(all_files ${comm_cpps} ${handle_cpps} ${main_cpps} ${project_headers} ${project_hppeaders})
source_group_by_dir(all_files ${src_path})

set(common_dir ${CMAKE_HOME_DIRECTORY}/comm)
set(third_dir ${CMAKE_HOME_DIRECTORY}/third)
set(mysql_dir ${third_dir}/mysql)
set(libevent_dir ${third_dir}/libevent)
set(xml_dir ${third_dir}/tinyxml)

set(curl_dir ${third_dir}/curl)
set(httpSvrComm_dir ${third_dir}/HttSvrComm)

#add include path
include_directories(${src_path})
include_directories(${common_dir})
include_directories(${common_dir}/Util)

include_directories(${curl_dir}/include)
include_directories(${httpSvrComm_dir}/include)
include_directories(${httpSvrComm_dir}/include/Log)
include_directories(${httpSvrComm_dir}/include/hiredis)

include_directories(${mysql_dir}/include)
include_directories(${libevent_dir}/include)
include_directories(${xml_dir}/include)
#include_directories(/usr/local/include)
link_directories(${mysql_dir}/lib)
link_directories(${libevent_dir}/lib)
link_directories(${xml_dir}/lib)
link_directories(${curl_dir}/lib)
link_directories(${httpSvrComm_dir}/lib)

IF (__DDCRYPTO__)
#cmake -D__DDCRYPTO__=ON ..
add_definitions("-D__DDCRYPTO__")
set(ssl_dir ${third_dir}/openssl)
set(DDCrypt_dir ${third_dir}/DDCrypt)
include_directories(${ssl_dir}/include)
include_directories(${DDCrypt_dir}/include)
link_directories(${ssl_dir}/linux_lib)
link_directories(${DDCrypt_dir}/lib)
ENDIF (__DDCRYPTO__)

add_definitions("-D__STDC_FORMAT_MACROS")
add_definitions("-DCURL_DISABLE_LDAP")

#add exec
add_executable(${proj_name}
	${all_files}
)

#target_link_libraries(${proj_name} -Wl,-Bstatic jsoncpp mysqlclient tinyxml hiredis event crypto DDCrypt -Wl,-Bdynamic pthread z rt)
IF (__DDCRYPTO__)
target_link_libraries(${proj_name} -Wl,-Bstatic HttpSvrComm mysqlclient tinyxml event DDCrypt crypto curl -Wl,-Bdynamic pthread z idn rt dl)
ELSE ()
target_link_libraries(${proj_name} -Wl,-Bstatic HttpSvrComm mysqlclient tinyxml event curl -Wl,-Bdynamic pthread z idn rt)
ENDIF ()