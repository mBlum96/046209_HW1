CXX = g++
CXXFLAGS = --std=c++11 -Wall -Werror -pedantic-errors -DNDBUG
CCLINK = $(CXX)
OBJS = smash.o commands.o signals.o
RM = rm -f

smash: $(OBJS)
	$(CCLINK) -o smash $(OBJS)

commands.o: commands.cpp commands.h
smash.o: smash.cpp commands.h signals.h
signals.o: signals.cpp sognals.h commands.h

clean:
	$(RM) $(TARGET) *.o *~ "#"* core.* smash
