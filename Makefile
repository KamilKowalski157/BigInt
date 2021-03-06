sources := $(wildcard *.cpp)
headers := ${wildcard *.h}
objects := $(sources:%.cpp=%.o)
target := $(shell pwd | xargs basename)

${target}: ${objects}
	g++ -g -std=c++20 ${objects}  -o ${target}

%.o: %.cpp ${headers}
	g++ -g -std=c++20 -c $<

clean:
	rm -f *.o
