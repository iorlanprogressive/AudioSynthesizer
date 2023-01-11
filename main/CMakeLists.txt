idf_component_register(
    SRCS main.c #Исходник
    REQUIRES driver freertos Timer ADC I2S Oscill Envelope LFO Delay Flanger Tremolo Vibrato Clipping PitchShifter VolumeAdjust #Необходимые компоненты
)
