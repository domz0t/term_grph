# Устанавливаем компилятор
CXX = g++

# Определяем флаги компиляции
CXXFLAGS = -Wall -g -std=c++17

# Исходные файлы для каждой программы
CUBE_SRCS = cube.cpp
BUTTERFLY_SRCS = butterfly.cpp
RUNE_SRCS = rune.cpp

# Объектные файлы для каждой программы
CUBE_OBJS = $(CUBE_SRCS:.cpp=.o)
BUTTERFLY_OBJS = $(BUTTERFLY_SRCS:.cpp=.o)
RUNE_OBJS = $(RUNE_SRCS:.cpp=.o)

# Цели по умолчанию (собирает все три программы)
all: cube buttefly rune

# Правило для компиляции программы из cube.cpp
cube: $(CUBE_OBJS)
	$(CXX) $(CXXFLAGS) -o cube $(CUBE_OBJS)

# Правило для компиляции программы из buttefly.cpp
buttefly: $(BUTTERFLY_OBJS)
	$(CXX) $(CXXFLAGS) -o buttefly $(BUTTERFLY_OBJS)

# Правило для компиляции программы из rune.cpp
rune: $(RUNE_OBJS)
	$(CXX) $(CXXFLAGS) -o rune $(RUNE_OBJS)

# Правило для компиляции исходных файлов в объектные файлы
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

# Правило для очистки сгенерированных файлов
clean:
	rm -f $(CUBE_OBJS) $(BUTTERFLY_OBJS) $(RUNE_OBJS) rune butterfly rune
