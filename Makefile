CXX = g++
CXXFLAGS = -std=c++17 -Wall

SRCS = src/Street.cpp \
       src/Intersection.cpp \
       src/RoadNetwork.cpp \
       src/Vehicle.cpp \
       src/Car.cpp \
       src/Motorcycle.cpp \
       src/Truck.cpp \
       src/Simulation.cpp \
       src/Logger.cpp

all:
	$(CXX) $(CXXFLAGS) -o app src/main.cpp $(SRCS)

tests:
	$(CXX) $(CXXFLAGS) -o tests src/tests.cpp $(SRCS)

clean:
	rm -f app tests
