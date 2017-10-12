add_executable(ReactionTimer
	${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

target_include_directories(ReactionTimer PRIVATE ${SFML_INCLUDE_DIR})
target_link_libraries(ReactionTimer sfml-system)
target_link_libraries(ReactionTimer sfml-window)
target_link_libraries(ReactionTimer sfml-graphics)
target_link_libraries(ReactionTimer sfml-network)
target_link_libraries(ReactionTimer sfml-audio)
