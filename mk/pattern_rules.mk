%.o: %.c
	$(CC) $(CFLAGS) $(__local_cflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(__local_cxxflags) $(CPPFLAGS) $(__local_cppflags) $(TARGET_ARCH) -c -o $@ $<


# disable antiquated and unused implicit rules
%: %.C
%: %.cc
%.c: %.l
%.c: %.w
%.c: %.w %.ch
%.c: %.y
%.dvi: %.tex
%.dvi: %.texi
%.dvi: %.texinfo
%.dvi: %.txinfo
%: %.f
%: %.F
%.f: %.F
%.f: %.m
%.f: %.r
%.info: %.texi
%.info: %.texinfo
%.info: %.txinfo
%.ln: %.c
%.ln: %.l
%.ln: %.y
%: %.m
%: %.mod
%.m: %.ym
%.o: %.C
%o: %.C
%.o: %.cc
%.o: %.f
%.o: %.F
%.o: %.m
%.o: %.mod
%.o: %.p
%.o: %.r
%.o: %.s
%.o: %.S
%: %.p
%.p: %.web
%: %.r
%:: RCS/%
%:: RCS/%,v
%.r: %.l
%: %.s
%:: s.%
%: %.S
%:: SCCS/s.%
%: %.sh
%.s: %.S
%.sym: %.def
%.tex: %.w
%.tex: %.web
