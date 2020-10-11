MODULE_CXXFLAGS  := -fno-rtti
MODULE_LIBRARIES := util

$(use-fmt)

$(call add-executable-module,$(get-path))
