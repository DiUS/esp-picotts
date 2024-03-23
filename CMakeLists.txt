set(PICOTTS_SRCS
  picopr.c
  picosa.c
  picokpr.c
  picodata.c
  picokfst.c
  picodbg.c
  picokdt.c
  picoctrl.c
  picoktab.c
  picoacph.c
  picokdbg.c
  #  picorsrc.c ### We use our custom esp_picorsrc.c instead
  picoos.c
  picospho.c
  picotok.c
  picosig.c
  picocep.c
  picofftsg.c
  picotrns.c
  picosig2.c
  picoextapi.c
  picopal.c
  picokpdf.c
  picobase.c
  picopam.c
  picowa.c
  picoklex.c
  picoapi.c
  picoknow.c
)
list(TRANSFORM PICOTTS_SRCS PREPEND "pico/lib/")


idf_component_register(
  SRCS
    "esp_picotts.c"
    "esp_picorsrc.c"
    ${PICOTTS_SRCS}
  INCLUDE_DIRS "include"
  PRIV_INCLUDE_DIRS "pico/lib"
  PRIV_REQUIRES "esp_partition"
)

# Suppress warnings in the library source
set_source_files_properties(
  ${PICOTTS_SRCS}
  PROPERTIES COMPILE_FLAGS
  "-Wno-implicit-fallthrough -Wno-unused-but-set-variable -Wno-unused-function"
)

# PicoTTS attempts to use exp() trickery which relies on a particular floating
# point representation format, which seemingly does not hold on Xtensa. As
# a workaround, we rename the picoos_quick_exp functin and provide our own
# wrapper back to standard math.h exp() instead.
set_source_files_properties(
  "pico/lib/picoos.c"
  PROPERTIES COMPILE_OPTIONS "-Dpicoos_quick_exp=picoos_quick_nope"
)

# Embed the correct language resources under known names
set(PICOTTS_TA_BIN "picotts_ta.bin")
set(PICOTTS_SG_BIN "picotts_sg.bin")
set(PICOTTS_LANG_DIR ${COMPONENT_DIR}/pico/lang)
set(PICOTTS_TA_BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PICOTTS_TA_BIN})
set(PICOTTS_SG_BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/${PICOTTS_SG_BIN})
if(CONFIG_PICOTTS_LANGUAGE_EN_GB)
  set(PICOTTS_TA_SRC "en-GB_ta.bin")
  set(PICOTTS_SG_SRC "en-GB_kh0_sg.bin")
elseif(CONFIG_PICOTTS_LANGUAGE_EN_US)
  set(PICOTTS_TA_SRC "en-US_ta.bin")
  set(PICOTTS_SG_SRC "en-US_lh0_sg.bin")
elseif(CONFIG_PICOTTS_LANGUAGE_DE_DE)
  set(PICOTTS_TA_SRC "de-DE_ta.bin")
  set(PICOTTS_SG_SRC "de-DE_gl0_sg.bin")
elseif(CONFIG_PICOTTS_LANGUAGE_ES_ES)
  set(PICOTTS_TA_SRC "es-ES_ta.bin")
  set(PICOTTS_SG_SRC "es-ES_zl0_sg.bin")
elseif(CONFIG_PICOTTS_LANGUAGE_FR_FR)
  set(PICOTTS_TA_SRC "fr-FR_ta.bin")
  set(PICOTTS_SG_SRC "fr-FR_nk0_sg.bin")
elseif(CONFIG_PICOTTS_LANGUAGE_IT_IT)
  set(PICOTTS_TA_SRC "it-IT_ta.bin")
  set(PICOTTS_SG_SRC "it-IT_cm0_sg.bin")
endif()
add_custom_command(OUTPUT ${PICOTTS_TA_BIN}
  COMMAND cp ${PICOTTS_LANG_DIR}/${PICOTTS_TA_SRC} ${PICOTTS_TA_BIN_PATH})
add_custom_command(OUTPUT ${PICOTTS_SG_BIN}
  COMMAND cp ${PICOTTS_LANG_DIR}/${PICOTTS_SG_SRC} ${PICOTTS_SG_BIN_PATH})
add_custom_target(picotts_ta_bin_gen DEPENDS ${PICOTTS_TA_BIN_PATH})
add_custom_target(picotts_sg_bin_gen DEPENDS ${PICOTTS_SG_BIN_PATH})
set_property(DIRECTORY "${COMPONENT_DIR}" APPEND PROPERTY
  ADDITIONAL_CLEAN_FILES ${PICOTTS_TA_BIN} ${PICOTTS_SG_BIN})

if(${CONFIG_PICOTTS_RESOURCE_MODE_PARTITION})
  partition_table_get_partition_info(part_ta_size "--partition-name ${CONFIG_PICOTTS_TA_PARTITION}" "size")
  partition_table_get_partition_info(part_ta_offs "--partition-name ${CONFIG_PICOTTS_TA_PARTITION}" "offset")
  if ("${part_ta_size}" AND "${part_ta_offs}")
    esptool_py_flash_to_partition(flash "${CONFIG_PICOTTS_TA_PARTITION}" "${PICOTTS_TA_BIN_PATH}")
  else()
    set(message "Failed to find ${CONFIG_PICOTTS_TA_PARTITION} in partition table")
  endif()
  partition_table_get_partition_info(part_sg_size "--partition-name ${CONFIG_PICOTTS_SG_PARTITION}" "size")
  partition_table_get_partition_info(part_sg_offs "--partition-name ${CONFIG_PICOTTS_SG_PARTITION}" "offset")
  if ("${part_sg_size}" AND "${part_sg_offs}")
    esptool_py_flash_to_partition(flash "${CONFIG_PICOTTS_SG_PARTITION}" "${PICOTTS_SG_BIN_PATH}")
  else()
    set(message "Failed to find ${CONFIG_PICOTTS_SG_PARTITION} in partition table")
  endif()
else()
  target_add_binary_data(
    ${COMPONENT_LIB} ${PICOTTS_TA_BIN_PATH} BINARY DEPENDS picotts_ta_bin_gen)
  target_add_binary_data(
    ${COMPONENT_LIB} ${PICOTTS_SG_BIN_PATH} BINARY DEPENDS picotts_sg_bin_gen)
endif()
