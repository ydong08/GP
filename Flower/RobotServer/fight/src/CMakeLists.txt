
set(proj_name FlowerRobot)
set(src_path
	"./"
)

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files ${src_path})

source_group("xml" FILES ${xml_files})

include_directories(${base_inc_path})
include_directories( ${sdk_inc_path}/comm/)
include_directories( ${sdk_inc_path}/comm/Protocol/)
include_directories( ${sdk_inc_path}/comm/Robot/)
include_directories(${third_inc_path})
include_directories(${sdk_inc_path}/third/libtolua/)
include_directories(${protocol_path})

include_directories(${include_targets})

include_directories(${figth_robot_src_path})
include_directories(${figth_robot_src_path}/admin)
include_directories(${figth_robot_src_path}/process)
include_directories(${figth_robot_src_path}/backconnect)
include_directories(${figth_robot_src_path}/model)

link_directories(${sdk_lib_path})

add_definitions(-DGAME_TYPE=E_FLOWER_GAME_ID)

add_executable(${proj_name}
	${all_files}
)

set_target_properties(${proj_name} PROPERTIES FOLDER "FlowerGame")
target_link_libraries(${proj_name} ${sdk_comm_libs} ${LIB_PRE_NAME}lua.${LIB_EXT_NAME} -Wl,-Bdynamic pthread dl)

set(fight_robot_output_path
		"${flower_game_out_path}/RobotServer/bin")
set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${fight_robot_output_path}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)