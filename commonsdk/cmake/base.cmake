
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(WIN32)
	set(LIB_EXT_NAME "lib")
	set(LIB_PRE_NAME "")
	set(DYN_EXT_NAME "dll")
	set(OS_NAME "nt")
else()
	set(LIB_EXT_NAME "a")
	set(LIB_PRE_NAME "lib")
	set(DYN_EXT_NAME "so")
	set(OS_NAME "linux")  # no osx now
endif()

if (WIN32)
else()
	set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-reorder -lpthread -ldl -lrt")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")
endif()

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

set(sdk_libs
    "${LIB_PRE_NAME}DD_BASE.${LIB_EXT_NAME}"
	"${LIB_PRE_NAME}hiredis.${LIB_EXT_NAME}"
	"${LIB_PRE_NAME}jsoncpp.${LIB_EXT_NAME}"
	"${LIB_PRE_NAME}tinyxml.${LIB_EXT_NAME}"
)