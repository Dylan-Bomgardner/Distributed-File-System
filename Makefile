CC = g++
CFLAGS = -Wall -Wextra -g

SRCS_DFS = dfs.cpp nethelp.cpp

SRCS_DFC = dfc.cpp

OBJS_DFS = $(SRCS_DFS:.cpp=.o)

OBJS_DFC = $(SRCS_DFC:.cpp=.o)

TARGET_DFS = dfs
TARGET_DFC = dfc

all: $(TARGET_DFS) $(TARGET_DFC)

$(TARGET_DFS): $(OBJS_DFS)
	$(CC) $(CFLAGS) -o $(TARGET_DFS) $(OBJS_DFS)

$(TARGET_DFC): $(OBJS_DFC)
	$(CC) $(CFLAGS) -o $(TARGET_DFC) $(OBJS_DFC)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS_DFS) $(OBJS_DFC) $(TARGET_DFS) $(TARGET_DFC)