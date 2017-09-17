cmake编译
	1、加密版本编译
	cmake -D__DDCRYPTO__=ON ..
	2、非加密版本编译
	cmake ..
visualGDB编译
	1、加密版本编译
	PREPROCESSOR_MACROS 添加 __DDCRYPTO__
	2、非加密版本编译
	PREPROCESSOR_MACROS 不加 __DDCRYPTO__