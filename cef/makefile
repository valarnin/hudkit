hudkit: main.cc
	@mkdir -p build
	clang++ -g \
		main.cc \
		hudkit.cc \
		browser_handler.cc \
		hudkit_window.cc \
		render_handler.cc \
		draw.cc \
		hudkit_config.cc \
		-o build/hudkit \
		-isystem ./cef-project \
		-Wl,-rpath,. \
		-I./cef-project \
		-L./cef-project/build/libcef_dll_wrapper \
		-L./cef-project/Release \
		`pkg-config --cflags --libs json-c keybinder-3.0 x11 gtk+-3.0` \
		-lcef_dll_wrapper \
		-lcef \
		-std=c++17
	cp -r cef-project/Release/*.so build/
	cp -r cef-project/Release/*.bin build/
	cp -r cef-project/Release/swiftshader build/
	cp -r cef-project/Resources/* build/

clean:
	rm -rf build
