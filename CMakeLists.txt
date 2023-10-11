cmake_minimum_required(VERSION 3.1...3.27)
project(
doneyet
VERSION 1.0
LANGUAGES CXX)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)

include(FindPkgConfig)

pkg_check_modules(NCURSES REQUIRED ncursesw)
#set ( CMAKE_C_FLAGS ${CURSES_CFLAGS} ) 

include_directories(${NCURSES_INCLUDE_DIRS})

#add_executable(doneyet config-parser.cc curses-menu.cc date.cc dialog-box.cc doneyet-config.cc file-manager.cc filter-predicate.cc hierarchical-list.cc info-box.cc list-chooser.cc main.cc note.cc project.cc serializer.cc task.cc utils.cc workspace.cc )

#target_link_options(doneyet PRIVATE ${NCURSES_LDFLAGS})
#target_link_libraries(doneyet ${NCURSES_LINK_LIBRARIES} -lpanelw -lmenuw -lformw -lncursesw)

add_executable(wide_input_test wide_input_test.cc)
target_link_options(wide_input_test PRIVATE ${NCURSES_LDFLAGS})
target_link_libraries(wide_input_test ${NCURSES_LINK_LIBRARIES} -lpanelw -lmenuw -lformw)

add_executable(wide_serializer_test wide_serializer_test.cc project.cc task.cc serializer.cc utils.cc date.cc note.cc hierarchical-list.cc dialog-box.cc)
target_link_options(wide_serializer_test PRIVATE ${NCURSES_LDFLAGS})
target_link_libraries(wide_serializer_test ${NCURSES_LINK_LIBRARIES} -lpanelw -lmenuw -lformw)

add_executable(serializer_test serializer_test.cc serializer.cc )
