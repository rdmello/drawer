all: main.run

main.run: main.cpp
	g++ -g -Wall -Werror $^ -lvterm -ltermbox -o $@

clean:
	rm -f ./*.run
