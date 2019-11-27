# All Targets
all: Splflix

# Tool invocations
Splflix: bin/Session.o bin/Action.o bin/User.o bin/Main.o bin/Watchable.o
	@echo 'Building target: Splflix'
	@echo 'Invoking: C++ Linker'
	g++ -o bin/Splflix bin/Session.o bin/Action.o bin/User.o bin/Main.o bin/Watchable.o
	@echo 'Finished building target: Splflix'
	@echo ' '

# Depends on the source and header files
bin/Session.o: src/Session.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Session.o src/Session.cpp

bin/User.o: src/User.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/User.o src/User.cpp

bin/Action.o: src/Action.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Action.o src/Action.cpp

bin/Main.o: src/Main.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Main.o src/Main.cpp

bin/Watchable.o: src/Watchable.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Watchable.o src/Watchable.cpp

#Clean the build directory
clean: 
	rm -f bin/*