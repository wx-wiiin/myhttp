TAR = main
OBJ = main.o pthread_list.o add2.o cnn_event.o 
OBJ += print_.o


CXX := g++
RM := rm -rf

$(TAR):$(OBJ)
	$(CXX) -g $^ -o $@ -pthread 

%.o:%.cpp
	$(CXX) -g -c $^ -o $@  -pthread 

.PHONY:
clean all:
	$(RM) $(TAR) $(OBJ)
clean:
	$(RM) $(TAR)
