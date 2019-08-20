CC = gcc

LDFLAGS =  -shared 
CFLAGS = 

SRCS = syslogmes.c syslogmes.o
APP  = syslogmes.dll

all: $(APP)


$(APP):  
	mc syslogmes.mc
	windres -o syslogmes.o syslogmes.rc
	$(CC) $(LDFLAGS) $(CFLAGS) $(SRCS) -o $(APP) 

 
