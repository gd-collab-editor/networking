cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

#add_library(collab-editor-networking STATIC ../common/action.cpp server.cpp ${ASIO_SRC} ${GDLEVEL_SRC})
add_executable(collab-editor-networking ../common/action.cpp server.cpp action_impls.cpp ${ASIO_SRC})
target_include_directories(collab-editor-networking PRIVATE
	../depends/asio/include
	../depends/websocketpp/websocketpp
	../depends/websocketpp/
	../depends/GDLevel/Include
	../common
)
target_link_directories(collab-editor-networking PRIVATE ../depends/GDLevel)
target_link_libraries(collab-editor-networking gdlevel)