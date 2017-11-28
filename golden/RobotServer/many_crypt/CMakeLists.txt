set(proj_name FlowerHundredRobotCrypt)

set(src_path
	"../hundred/src"
)

file(GLOB_RECURSE project_headers ${src_path}/*.h)
file(GLOB_RECURSE project_cpps ${src_path}/*.cpp)
file(GLOB_RECURSE project_c ${src_path}/*.c)
set(all_files ${project_headers} ${project_cpps} ${project_c})
source_group_by_dir(all_files ${src_path})

include_directories(${base_inc_path})
include_directories( ${sdk_inc_path}/comm/)
include_directories( ${sdk_inc_path}/comm/Protocol/)
include_directories( ${sdk_inc_path}/comm/Robot/)
include_directories(${third_inc_path})
include_directories(${sdk_inc_path}/third/libtolua/)
include_directories(${protocol_path})

include_directories(${src_path})
include_directories(${src_path}/process)
include_directories(${src_path}/model)
include_directories(${src_path}/backconnect)
include_directories(${src_path}/admin)

add_definitions(-DGAME_TYPE=E_FLOWER_GAME_ID)

set(crypt_lib_inc_path
	"../../../DDCrypt_SDK/include"
	"../../../DDCrypt_SDK/openssl/include"
	"../../../DDCrypt_SDK/third/curl/include"
)
include_directories(${crypt_lib_inc_path})

link_directories(${sdk_lib_path})

set(crypt_lib_path
	"../../../DDCrypt_SDK/LibCentos"
	"../../../DDCrypt_SDK/TestSdk/openssl/linux_lib"
	"../../../DDCrypt_SDK/third/curl/lib"
)
link_directories(${crypt_lib_path})

add_definitions(-DCRYPT)

add_executable(${proj_name}
	${all_files}
)
set_target_properties(${proj_name} PROPERTIES FOLDER "FlowerGame")

set(crypt_libs
	"${LIB_PRE_NAME}DDCrypt.${LIB_EXT_NAME}"
    "${LIB_PRE_NAME}curl.${LIB_EXT_NAME}"
	"${LIB_PRE_NAME}ssl.${LIB_EXT_NAME}"
	"${LIB_PRE_NAME}crypto.${LIB_EXT_NAME}"
)
target_link_libraries(${proj_name} ${sdk_comm_crypt_libs} ${crypt_libs} -Wl,-Bdynamic pthread dl)

# set output dir
set(robot_output_dir
	"${flower_game_out_path}/RobotServer/hundred/bin"
)
set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${robot_output_dir}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)
