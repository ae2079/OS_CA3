CC = g++
CFLAGS = -std=c++11 -pthread -Wall -Werror

EXECUTABLE_FILE = ImageFilters.out

SensitivityList = readImg.cpp

all: $(EXECUTABLE_FILE)

$(EXECUTABLE_FILE): $(SensitivityList)
	$(CC) $(CFLAGS) $(SensitivityList) -o $(EXECUTABLE_FILE)

.PHONY: clean
clean:
	rm *.out
