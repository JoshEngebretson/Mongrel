get_target_property(VLUMPY vlumpy LOCATION)
get_target_property(ACC acc LOCATION)

add_subdirectory(common)
add_subdirectory(doom)
add_subdirectory(doom1)
add_subdirectory(doom2)
add_subdirectory(hexen)

install(FILES games.txt DESTINATION ${DATADIR}/basev)
