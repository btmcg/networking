MODULE_CXXFLAGS  := -fno-rtti
MODULE_LIBRARIES := util

$(call add-executable-module,$(get-path))
