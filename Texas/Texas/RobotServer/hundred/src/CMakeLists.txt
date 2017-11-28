
set(proj_name TexasHundredRobot)
set(src_path
	"../../../../Common/RobotServer/hundred/src"
)

file(GLOB_RECURSE project_headers ${src_path}/*.h)
file(GLOB_RECURSE project_cpps ${src_path}/*.cpp)
file(GLOB_RECURSE project_c ${src_path}/*.c)
set(all_files ${project_headers} ${project_cpps} ${project_c})
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

add_definitions(-DGAME_TYPE=E_TEXAS_GAME_ID)
add_definitions(-Droom_config_name="TexasMany")
add_definitions(-Drobot_win_many_name="RobotWinManyTexas")

add_executable(${proj_name}
	${all_files}
)

set_target_properties(${proj_name} PROPERTIES FOLDER "TexasGame")

target_link_libraries(${proj_name} ${sdk_comm_libs} -Wl,-Bdynamic pthread)
# set output dir
set(hundred_robot_output_dir "${texas_game_out_path}/RobotServer/bin")

set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${hundred_robot_output_dir}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)