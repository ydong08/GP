

# sdk 
set(sdk_inc_path
    ${sdk_path}/include
)
set(base_inc_path
    ${sdk_inc_path}/base/
    ${sdk_inc_path}/base/Log
    ${sdk_inc_path}/base/Network
    ${sdk_inc_path}/base/Util
)
set(third_inc_path
    ${sdk_inc_path}/third/hiredis/
    ${sdk_inc_path}/third/jsoncpp/
    ${sdk_inc_path}/third/lua-5.1.5/
    ${sdk_inc_path}/third/tinyxml/
)

set(comm_inc_path
    ${sdk_inc_path}/comm/
    ${sdk_inc_path}/comm/Agent/
    ${sdk_inc_path}/comm/Game/
    ${sdk_inc_path}/comm/GameProc/
    ${sdk_inc_path}/comm/Protocol/
)

set(gtest_inc_path
    ${sdk_inc_path}/third/googletest/
)

set(sdk_lib_path
    ${sdk_path}/${OS_NAME}/lib
)

set(sdk_comm_libs
    "${LIB_PRE_NAME}DD_COMM.${LIB_EXT_NAME}"
    ${sdk_libs}
)

set(sdk_comm_crypt_libs
    "${LIB_PRE_NAME}DD_COMM_CRYPT.${LIB_EXT_NAME}"
    ${sdk_libs}
)

set(test_output_dir
	"${CMAKE_HOME_DIRECTORY}/Output/UnitTest/"
)

set(gtest_libs
    "${LIB_PRE_NAME}gtest.${LIB_EXT_NAME}"
    "${LIB_PRE_NAME}gtest_main.${LIB_EXT_NAME}"
)