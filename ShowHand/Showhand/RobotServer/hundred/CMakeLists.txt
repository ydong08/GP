
set(proj_name ShowhandHundredRobot)

set(src_path
	"../../../Common/RobotServer/hundred/src"
)

file(GLOB_RECURSE project_headers ${src_path}/*.h)
file(GLOB_RECURSE project_cpps ${src_path}/*.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files ${src_path})

include_directories(${base_inc_path})
include_directories(${comm_inc_path})
include_directories(${third_inc_path})
include_directories( ${sdk_inc_path}/comm/Robot/)

include_directories(${src_path})
include_directories(${src_path}/admin)
include_directories(${src_path}/process)
include_directories(${src_path}/backconnect)
include_directories(${src_path}/model)

link_directories(${sdk_lib_path})

add_definitions(-DGAME_TYPE=E_SHOWHAND_GAME_ID)
add_definitions(-Droom_config_name="ShowhandMany")
add_definitions(-Drobot_win_many_name="RobotWinManyShowwhand")

add_executable(${proj_name} ${all_files})

target_link_libraries(${proj_name} ${sdk_comm_libs} -Wl,-Bdynamic pthread)

set_target_properties(${proj_name} PROPERTIES FOLDER "ShowhandGame")


# set output dir
set(hundred_output_path "${showhand_game_out_path}/RobotServer/hundred/bin")

set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${hundred_output_path}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)