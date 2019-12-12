NAME := test

INCDIR := include
OBJDIR := obj

ARCH :=`root-config --arch`

HEADERS  := 
SOURCES  :=  main.C
OBJS     :=  $(OBJDIR)/main.o

DEFINES  := -I. -I$(INCDIR) -I$(OBJDIR) -I`root-config --cflags`
#DEFINES  += -I ../include
#DEFINES  += -I/afs/ihep.ac.cn/users/c/chenqh/x/WFCTA/include
DEFINES  += -I/afs/ihep.ac.cn/users/h/hliu/Documents/LHAASO/WFCTA/include

#CXXFLAGS += -D__DST_HE_12__
CXXFLAGS := -O3 -fPIC -Wno-write-strings
#CXXFLAGS += -D_THIN_

LDFLAGS  := `root-config --libs` 
#LDFLAGS  += -lCore -lMinuit -lNetx -lProof -lProofPlayer -lTMVA -lXMLIO -lMLP -lTreePlayer -lRFIO -lXrdClient -lGpad -lNet -lHist -lHistPainter -lGraf -lMatrix

#LDFLAGS  +=-L. /afs/ihep.ac.cn/users/h/hliu/Documents/LHAASO/WFCTA/lib/lib.so
#LDFLAGS   +=-L. ../lib/lib.so
#LDFLAGS   +=-L. /afs/ihep.ac.cn/users/c/chenqh/x/WFCTA/lib/lib.so
LDFLAGS   +=-L. /afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/Noise/lib.so

$(NAME): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) 

$(OBJDIR)/main.o: main.C
	#mkdir $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

$(OBJDIR)/%.o: $(OBJDIR)/%.C
	$(CXX) $(CXXFLAGS) $(DEFINES) -c $^ -o $@

clean:
	rm -fv $(OBJDIR)/* $(NAME)

