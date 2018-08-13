CC = g++
PIE_OPT = -pie -fPIE -fstack-protector-all -Wl,-z,relro,-z,now


gg: main.o msgqueue.o expr.o hgmp.o
	$(CC) -o gg main.o msgqueue.o expr.o hgmp.o -lpthread $(PIE_OPT)

main.o: main.cpp
	$(CC) -c main.cpp msgqueue.h expr.h -lpthread $(PIE_OPT)

msgqueue.o: msgqueue.cpp msgqueue.h
	$(CC) -c msgqueue.cpp $(PIE_OPT)

expr.o: expr.cpp expr.h hgmp.h
	$(CC) -c expr.cpp  $(PIE_OPT)

hgmp.o: hgmp.cpp hgmp.h
	$(CC) -c hgmp.cpp $(PIE_OPT)
