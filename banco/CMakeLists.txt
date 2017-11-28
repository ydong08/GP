
project(BaccaratGame)

set(baccarat_game_src_path
	"${CMAKE_HOME_DIRECTORY}/BaccaratGame/"
)

set(baccarat_game_out_path
	"${CMAKE_HOME_DIRECTORY}/Output/BaccaratGame/"
)

option(BUILD_BACCARAT_HUNDRED_ALLOC "build baccarat hundred alloc server" TRUE)
if (BUILD_BACCARAT_HUNDRED_ALLOC)
	# add alloc server
	set(alloc_src_path
		"${baccarat_game_src_path}/AllocServer/many"
	)
	add_subdirectory(${alloc_src_path})
endif()

option(BUILD_BACCARAT_ALLOC "build baccarat alloc server" TRUE)
if (BUILD_BACCARAT_ALLOC)
	# add alloc server
	set(alloc_src_path
			"${baccarat_game_src_path}/AllocServer/trunk"
			)
	add_subdirectory(${alloc_src_path})
endif()

option(BUILD_BACCARAT_GAME "build baccarat game server" TRUE)
if (BUILD_BACCARAT_GAME)
	# add game server
	set(hundred_src_path
		"${baccarat_game_src_path}/GameServer/trunk/src"
	)
	add_subdirectory(${hundred_src_path})
endif()

option(BUILD_BACCARAT_HANDRED_GAME "build baccarat hundred game server" TRUE)
if (BUILD_BACCARAT_HANDRED_GAME)
	# add game server
	set(hundred_src_path
			"${baccarat_game_src_path}/GameServer/many/src"
			)
	add_subdirectory(${hundred_src_path})
endif()

option(BUILD_BACCARAT_ROBOT "build baccarat robot server" TRUE)
if (BUILD_BACCARAT_ROBOT)
	# add robot server
	set(hundred_robot_src_path
		"${baccarat_game_src_path}/RobotServer/trunk/src"
	)
	add_subdirectory(${hundred_robot_src_path})
endif()

option(BUILD_BACCARAT_ROBOT_CRYPT "build baccarat robot crypt server" TRUE)
if (BUILD_BACCARAT_ROBOT_CRYPT)
	# add robot server
	set(hundred_robot_crypt_src_path
		"${baccarat_game_src_path}/RobotServerCrypt/"
	)
	add_subdirectory(${hundred_robot_crypt_src_path})
endif()