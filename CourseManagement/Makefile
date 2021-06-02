CC=/usr/local/bin/gcc
CXX=/usr/local/bin/g++
OBJS = main.o Exceptions.o Graph.o Node.o 
TAR = Makefile main.cpp Graph.cpp Graph.h Exceptions.cpp Exceptions.h Node.cpp Node.h Edge.h graph.i test_in.txt test_out.txt design.pdf
EXEC = gcalc
TEMP = a.txt b.txt c.txt a   g.txt
DEBUG_FLAG = -DNDBUG
COMP_FLAG = -std=c++11 --pedantic-errors -Wall -Werror -fPIC
$(EXEC) : $(OBJS)
	$(CXX) $(DEBUG_FLAG) $(OBJS) -o $@
main.o: main.cpp Graph.h Exceptions.h Node.h Edge.h
	$(CXX) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp	
Exceptions.o: Exceptions.cpp Exceptions.h
	$(CXX) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp
Graph.o: Graph.cpp Graph.h Exceptions.h Node.h Edge.h
	$(CXX) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp
Node.o: Node.cpp Node.h Exceptions.h
	$(CXX) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.cpp
libgraph.a: $(OBJS) 
	ar -rs $@ $^ 
tar: $(TAR)
	zip 208951905 $^ 	
clean:
	rm -f $(OBJS) $(EXEC) $(TEMP)