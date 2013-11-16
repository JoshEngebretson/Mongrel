configure_file(basepak.ls.in ${CMAKE_CURRENT_BINARY_DIR}/basepak.ls)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/basepak.pk3
	COMMAND ${VLUMPY} ARGS basepak.ls
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/basepak.ls vlumpy
		basepak.ls.in
		confont.pcx
		decorate.txt
		default.cfg
		in_epi1.txt
		in_epi2.txt
		in_epi3.txt
		in_htc1.txt
		in_htc2.txt
		in_htc3.txt
		language.en
		line_specials.txt
		lockdefs.txt
		reverbs.txt
		startup.vs
		terrain.txt
		textcolo.txt
		vavoom_decorate_defs.xml
		vfxdefs.txt
		x11r6rgb.txt
		actors/constants.txt
		actors/nativeclasses.txt
		actors/shared/blood.txt
		actors/shared/debris.txt
		actors/shared/fountain.txt
		actors/shared/pickups.txt
		actors/shared/sharedmisc.txt
		actors/shared/soundsequence.txt
		actors/shared/splashes.txt
		actors/hexen/fighterplayer.txt
		graphics/mc_arrow.png
		sounds/dsempty.lmp
		sounds/dsquake.flac
		sounds/icebrk1a.flac
		sounds/icedth1.flac
		sounds/railgf1.flac
		sounds/spark1.flac
		sounds/spark2.flac
		sounds/spark3.flac
		sprites/iceca0.png
		sprites/icecb0.png
		sprites/icecc0.png
		sprites/icecd0.png
		sprites/tnt1a0.lmp
		sprites/unkna0.png
		textures/-noflat-.png
		${CMAKE_SOURCE_DIR}/progs/common/engine/*.vc
		${CMAKE_SOURCE_DIR}/progs/common/linespec/*.vc
		${CMAKE_CURRENT_SOURCE_DIR}/glshaders/*.fs
		${CMAKE_CURRENT_SOURCE_DIR}/glshaders/*.vs
	)
add_custom_target(common_basepak ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/basepak.pk3)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/basepak.pk3
	DESTINATION ${DATADIR}/basev/common)
