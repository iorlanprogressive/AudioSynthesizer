idf_component_register(
	SRCS "Oscill.c"
	INCLUDE_DIRS "include"
	REQUIRES Timer Delay Flanger Tremolo Clipping VolumeAdjust I2S
)
