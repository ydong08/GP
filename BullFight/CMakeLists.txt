
project(BullFight2017)

set(bull_game_src_path
	"${CMAKE_HOME_DIRECTORY}/BullFight2017/"
)

set(bull_game_out_path
	"${CMAKE_HOME_DIRECTORY}/Output/BullFight2017/"
)

option(BUILD_BULL2017_ALLOC "build bull alloc server" TRUE)
if (BUILD_BULL2017_ALLOC)
	# add alloc server
	set(alloc_src_path
		"${bull_game_src_path}/AllocServer/"
	)
	add_subdirectory(${alloc_src_path})
endif()

option(BUILD_BULL2017_GAME "build bull hundred game server" TRUE)
if (BUILD_BULL2017_GAME)
	# add hundred server
	set(hundred_src_path
		"${bull_game_src_path}/GameServer/many/src"
	)
	add_subdirectory(${hundred_src_path})
endif()

option(BUILD_BULL2017_ROBOT "build hundred robot server" TRUE)
if (BUILD_BULL2017_ROBOT)
	# add hundred robot server
	set(hundred_robot_src_path
		"${bull_game_src_path}/RobotServer/many/src"
	)
	add_subdirectory(${hundred_robot_src_path})
endif()