add_executable(lab9_m1.elf main_m1.c bird.c)
target_link_libraries(lab9_m1.elf ${330_LIBS} interrupts touchscreen intervalTimer)
set_target_properties(lab9_m1.elf PROPERTIES LINKER_LANGUAGE CXX)

add_executable(lab9_m2.elf main_m2.c pipes.c)
target_link_libraries(lab9_m2.elf ${330_LIBS} interrupts touchscreen intervalTimer)
set_target_properties(lab9_m2.elf PROPERTIES LINKER_LANGUAGE CXX)

add_executable(lab9_m3.elf main_m3.c pipes.c bird.c gamecontrol.c)
target_link_libraries(lab9_m3.elf ${330_LIBS} interrupts touchscreen intervalTimer buttons_switches)
set_target_properties(lab9_m3.elf PROPERTIES LINKER_LANGUAGE CXX)

add_executable(lab9.elf main_final.c pipes.c bird.c gamecontrol.c)
target_link_libraries(lab9.elf ${330_LIBS} interrupts touchscreen intervalTimer buttons_switches)
set_target_properties(lab9.elf PROPERTIES LINKER_LANGUAGE CXX)