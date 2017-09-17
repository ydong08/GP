
set(proj_name ShowhandGame)

set(src_path
	"./"
)

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files ${src_path})

source_group("xml" FILES ${xml_files})

include_directories(${include_targets})

include_directories(${fight_src_path})
include_directories(${fight_src_path}/admin)
include_directories(${fight_src_path}/process)
include_directories(${fight_src_path}/backserver)
include_directories(${fight_src_path}/hallmanager)
include_directories(${fight_src_path}/model)

add_executable(${proj_name} ${all_files})

target_link_libraries(${proj_name} ${link_targets})

set_target_properties(${proj_name} PROPERTIES FOLDER "ShowhandGame")


# set output dir
set(fight_output_path "${showhand_game_out_path}/GameServer/fight/bin")

set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${fight_output_path}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)