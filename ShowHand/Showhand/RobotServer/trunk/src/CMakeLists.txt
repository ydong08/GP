
set(proj_name ShowhandFightRobot)

set(src_path
	"./"
)

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files ${src_path})

include_directories(${include_targets})

include_directories(${robot_src_path})
include_directories(${robot_src_path}/admin)
include_directories(${robot_src_path}/process)
include_directories(${robot_src_path}/backconnect)
include_directories(${robot_src_path}/model)

add_executable(${proj_name}
	${all_files}
)

set_target_properties(${proj_name} PROPERTIES FOLDER "ShowhandGame")
target_link_libraries(${proj_name} ${link_targets})

# set output dir
set(robot_output_path "${showhand_game_out_path}/RobotServer/fight/bin")

set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${robot_output_path}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)