# Copyright(c) 2020-present, Brian McGuire.
# Distributed under the BSD-2-Clause
# (http://opensource.org/licenses/BSD-2-Clause)


# pattern rules for c
%.o: %.c
	$(CC) $(CFLAGS) $(__local_cflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) -c $< -o $@

%.d: %.c
	$(CC) -MM -MT $(@:d=o) $(CFLAGS) $(__local_cflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) $< -o $@

# pattern rules for c++
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(__local_cxxflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) -c $< -o $@

%.d: %.cpp
	$(CXX) -MM -MT $(@:d=o) $(CXXFLAGS) $(__local_cxxflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) $< -o $@
