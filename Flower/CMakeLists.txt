
project(FlowerGame)

set(flower_game_src_path
	"${CMAKE_HOME_DIRECTORY}/FlowerGame/"
)

set(flower_game_out_path
	"${CMAKE_HOME_DIRECTORY}/Output/FlowerGame/"
)

option(BUILD_FLOWER_ALLOC "build flower alloc server" TRUE)
if (BUILD_FLOWER_ALLOC)
	# add alloc server
	set(alloc_src_path
		"${flower_game_src_path}/AllocServer/"
	)
	add_subdirectory(${alloc_src_path})
endif()

option(BUILD_FLOWER_FIGHT_GAME "build flower fight game server" TRUE)
if (BUILD_FLOWER_FIGHT_GAME)
	# add fight server
	set(fight_src_path
		"${flower_game_src_path}/GameServer/fight/src"
	)
	add_subdirectory(${fight_src_path})
endif()

option(BUILD_FLOWER_HUNDRED_GAME "build flower hundred game server" TRUE)
if (BUILD_FLOWER_HUNDRED_GAME)
	# add hundred server
	set(hundred_src_path
		"${flower_game_src_path}/GameServer/hundred/src"
	)
	add_subdirectory(${hundred_src_path})
endif()

option(BUILD_FLOWER_FIGHT_ROBOT "build fight robot server" TRUE)
if (BUILD_FLOWER_FIGHT_ROBOT)
	#set fight src path
	set(figth_robot_src_path
		"${flower_game_src_path}/RobotServer/fight/src"
	)
	add_subdirectory(${figth_robot_src_path})
endif()

option(BUILD_FLOWER_HUNDRED_ROBOT "build hundred robot server" TRUE)
if (BUILD_FLOWER_HUNDRED_ROBOT)
	# add hundred robot server
	set(hundred_robot_src_path
		"${flower_game_src_path}/RobotServer/hundred/src"
	)
	add_subdirectory(${hundred_robot_src_path})
endif()

option(BUILD_FLOWER_HUNDRED_ROBOT_CRYPT "build crypt hundred  robot server" TRUE)
if (BUILD_FLOWER_HUNDRED_ROBOT_CRYPT)
	# add hundred robot server
	set(crpty_hundred_robot_src_path
		"${flower_game_src_path}/RobotServer/many_crypt"
	)
	add_subdirectory(${crpty_hundred_robot_src_path})
endif()

