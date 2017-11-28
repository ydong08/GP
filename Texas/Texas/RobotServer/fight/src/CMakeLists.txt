set(proj_name TexasRobot)

set(src_path
	"./"
)

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files ${src_path})

include_directories(${include_targets})
include_directories(${src_path})

include_directories(${protocol_path})
include_directories(${base_inc_path})
include_directories( ${sdk_inc_path}/comm/)
include_directories( ${sdk_inc_path}/comm/Protocol/)
include_directories( ${sdk_inc_path}/comm/Robot/)
include_directories(${third_inc_path})
include_directories(${src_path}/process)
include_directories(${src_path}/model)
include_directories(${src_path}/backconnect)
include_directories(${src_path}/admin)

link_directories(${sdk_lib_path})

add_executable(${proj_name}
	${all_files}
)
target_link_libraries(${proj_name} ${sdk_comm_libs} -Wl,-Bdynamic pthread)

set_target_properties(${proj_name} PROPERTIES FOLDER "TexasGame")

# set output dir
set(robot_output_dir
	"${texas_game_out_path}/RobotServer/bin"
)
set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${robot_output_dir}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)