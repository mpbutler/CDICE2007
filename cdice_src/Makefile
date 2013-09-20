all: cdice

CDICEInput.o: CDICE.h CDICEInput.cpp
	g++ -c -o CDICEInput.o CDICEInput.cpp
	
CDICE.o: CDICE.h CDICE.cpp
	g++ -c -o CDICE.o CDICE.cpp
	
cdice: CDICEMain.cpp CDICEInput.o CDICE.o
	g++ -o cdice CDICEMain.cpp CDICEInput.o CDICE.o

clean:
	rm cdice
	rm CDICEInput.o
	rm CDICE.o

