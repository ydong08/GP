
set(proj_name FlowerHundredGame)
set(src_path
	"./"
)

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files ${src_path})

include_directories(${base_inc_path})
include_directories(${comm_inc_path})
include_directories(${third_inc_path})
include_directories(${protocol_path})

include_directories(${include_targets})
include_directories(${hundred_src_path})
include_directories(${lua_path})
#include_directories(${robot_src_path}/util)
include_directories(${hundred_src_path}/admin)
include_directories(${hundred_src_path}/backserver)
include_directories(${hundred_src_path}/hallmanager)
include_directories(${hundred_src_path}/process)
include_directories(${hundred_src_path}/model)

link_directories(${sdk_lib_path})
link_directories(${lua_lib_path})
link_directories(${jsoncpp_lib_path})
link_directories(${tinyxml_output_dir})

add_definitions(-DGAME_TYPE=E_FLOWER_GAME_ID)

add_executable(${proj_name}
	${all_files}
)
target_link_libraries(${proj_name}
	${hiredis_path}${hiredis_lib}
	${dlnet_lib_path}${dlnet_lib}
    ${lua_lib}
	${json_lib_name}
)
set_target_properties(${proj_name} PROPERTIES FOLDER "FlowerGame")
#target_link_libraries(${proj_name} ${link_targets})
target_link_libraries(${proj_name} ${sdk_comm_libs} -Wl,-Bdynamic pthread)

# set output dir
set(hundred_output_dir
	"${flower_game_out_path}/GameServer/bin"
)
set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${hundred_output_dir}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)