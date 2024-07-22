add_library(
	sqlatestfeatures

	main.c
	console.c
	functions.c
)

# https://stackoverflow.com/a/19310455/14197165
# Uncomment to force 32-bit compilation.
#set_target_properties(sqlatestfeatures PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
