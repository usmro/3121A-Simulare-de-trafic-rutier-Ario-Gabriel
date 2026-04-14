CXX = g++
CXXFLAGS = -std=c++17 -Wall -Isrc

# all .cpp files in src/
SRCS = src/main.cpp \
       src/Street.cpp \
       src/Intersection.cpp \
       src/RoadNetwork.cpp \
       src/Vehicle.cpp \
       src/Car.cpp \
       src/Motorcycle.cpp \
       src/Truck.cpp \
       src/Simulation.cpp \
       src/Logger.cpp

TARGET = app

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
