
project(ShowhandGame)

set(showhand_game_out_path
	"${CMAKE_HOME_DIRECTORY}/Output/ShowhandGame/"
)

option(BUILD_SHOWHAND_ALLOC "build showhand alloc server" TRUE)
if (BUILD_SHOWHAND_ALLOC)
    # add alloc server
    set(alloc_src_path
        "${showhand_path}/AllocServer/"
    )
    add_subdirectory(${alloc_src_path})
endif()

#option(BUILD_SHOWHAND_FIGHT_GAME "build showhand game server" TRUE)
#if (BUILD_SHOWHAND_FIGHT_GAME)
        # add game server
#    set(fight_src_path
#        "${showhand_path}/GameServer/trunk/src"
#    )
#    add_subdirectory(${fight_src_path})
#endif()

option(BUILD_SHOWHAND_HUNDRED_GAME "build showhand hundred game server" TRUE)
if (BUILD_SHOWHAND_HUNDRED_GAME)
	# add game server
	set(hundred_src_path
		"${showhand_path}/GameServer/hundred/"
	)
	add_subdirectory(${hundred_src_path})
endif()

#option(BUILD_SHOWHAND_ROBOT "build showhand robot server" TRUE)
#if (BUILD_SHOWHAND_ROBOT)
    # add robot server
#    set(robot_src_path
#        "${showhand_path}/RobotServer/trunk/src"
#	)
#    add_subdirectory(${robot_src_path})
#endif()

option(BUILD_SHOWHAND_HUNDRED_ROBOT "build showhand hundred robot server" TRUE)
if (BUILD_SHOWHAND_HUNDRED_ROBOT)
    # add robot server
    set(robot_src_path
        "${showhand_path}/RobotServer/hundred"
	)
    add_subdirectory(${robot_src_path})
endif()

option(BUILD_SHOWHAND_HUNDRED_ROBOT_CRYPT "build showhand hundred robot crypt server" TRUE)
if (BUILD_SHOWHAND_HUNDRED_ROBOT_CRYPT)
    # add robot server
    set(robot_crypt_src_path
        "${showhand_path}/RobotServer/hundred_crypt"
	)
    add_subdirectory(${robot_crypt_src_path})
endif()