set(proj_name TexasGame)

set(src_path
	"./"
)

file(GLOB_RECURSE project_headers *.h)
file(GLOB_RECURSE project_cpps *.cpp)
set(all_files ${project_headers} ${project_cpps})
source_group_by_dir(all_files ${src_path})

source_group("xml" FILES ${xml_files})

include_directories(${base_inc_path})
include_directories(${comm_inc_path})
include_directories(${third_inc_path})
include_directories(${protocol_path})

include_directories(${src_path})
include_directories(${src_path}/process)
include_directories(${src_path}/admin)
include_directories(${src_path}/model)

link_directories(${sdk_lib_path})

add_executable(${proj_name}
	${all_files}
)

target_link_libraries(${proj_name} ${sdk_comm_libs} -Wl,-Bdynamic pthread)
set_target_properties(${proj_name} PROPERTIES FOLDER "TexasGame")

# set output dir
set(game_output_dir
	"${texas_game_out_path}/GameServer/bin"
)
set_target_properties(${proj_name}
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${game_output_dir}
	ARCHIVE_OUTPUT_DIRECTORY ${lib_out_dir}
	LIBRARY_OUTPUT_DIRECTORY ${lib_out_dir}
)