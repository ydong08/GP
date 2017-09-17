
project(TexasGame)

set(texas_game_src_path
	"./"
)

set(texas_game_out_path
	"${CMAKE_HOME_DIRECTORY}/Output/TexasGame/"
)

option(BUILD_TEXAS_ALLOC "build texas alloc server" TRUE)
if (BUILD_TEXAS_ALLOC)
	# add alloc server
	set(alloc_src_path
		"${texas_game_src_path}/AllocServer/trunk/src"
	)
	add_subdirectory(${alloc_src_path})
endif()

option(BUILD_TEXAS_FIGHT_GAME "build texas game server" TRUE)
if (BUILD_TEXAS_FIGHT_GAME)
	# add game server
	set(fight_src_path
		"${texas_game_src_path}/GameServer/fight/src"
	)
	add_subdirectory(${fight_src_path})
endif()

option(BUILD_TEXAS_FIGHT_ROBOT "build texas robot server" TRUE)
if (BUILD_TEXAS_FIGHT_ROBOT)
	# add robot server
	set(fight_robot_src_path
		"${texas_game_src_path}/RobotServer/fight/src"
	)
	add_subdirectory(${fight_robot_src_path})
endif()

option(BUILD_TEXAS_HUNDRED_GAME "build texas hundred game server" TRUE)
if (BUILD_TEXAS_HUNDRED_GAME)
	# add hundred server
	set(hundred_src_path
		"${texas_game_src_path}/GameServer/hundred/src"
	)
	add_subdirectory(${hundred_src_path})
endif()

option(BUILD_TEXAS_HUNDRED_ROBOT "build texas hundred robot server" TRUE)
if (BUILD_TEXAS_HUNDRED_ROBOT)
	# add hundred robot server
	set(hundred_robot_src_path
		"${texas_game_src_path}/RobotServer/hundred/src"
	)
	add_subdirectory(${hundred_robot_src_path})
endif()
