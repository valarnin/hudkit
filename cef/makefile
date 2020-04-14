hudkit: main.cc
	@mkdir -p build
	clang++ \
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
	cp fetch-required-files.sh build/
	cd build && ./fetch-required-files.sh

clean:
	rm build/hudkit
	rm build/fetch-required-files.sh
	rm build/release.tar.gz

dist:
	cd build && tar zcf release.tar.gz hudkit fetch-required-files.sh
