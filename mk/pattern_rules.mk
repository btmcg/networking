%.o: %.c
	$(CC) $(CFLAGS) $(__local_cflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(__local_cxxflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) -c -o $@ $<
