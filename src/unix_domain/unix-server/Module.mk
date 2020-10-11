MODULE_CXXFLAGS := -fno-rtti

$(use-fmt)

$(call add-executable-module,$(get-path))
