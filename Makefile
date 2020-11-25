CXX = aarch64-openwrt-linux-g++
TARGET	= canfd

DIR		= . 
INC		= -I. 
CFLAGS	= -Os -Wall -std=c++14
LDFLAGS =

OBJPATH	= .

FILES	= $(foreach dir,$(DIR),$(wildcard $(dir)/*.cpp))

OBJS	= $(patsubst %.cpp,%.o,$(FILES))

all:$(OBJS) $(TARGET)

$(OBJS):%.o:%.cpp
	$(CXX) $(CFLAGS) $(INC) -c -o $(OBJPATH)/$(notdir $@) $< 

$(TARGET):$(OBJPATH)
	$(CXX)  $(OBJPATH)/*.o -o $@ $(LDFLAGS)

install:
	cp $(TARGET) $(DESTDIR)/usr/bin/

clean:
	-rm -f *.o
	-rm -f $(TARGET)
	
