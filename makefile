CFLAGS = -g -Wall -Werror -Wfatal-errors -pedantic -std=c++11
LFLAGS = 
CC     = c++

srcs = $(wildcard *.cpp)
objs = $(srcs:.cpp=.o)
deps = $(srcs:.cpp=.d)

shiny_apple: $(objs)
	$(CC) $^ -o $@ $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

# $(RM) is rm -f by default
clean:
	$(RM) $(objs) $(deps) a.out shiny_apple core

-include $(deps)
