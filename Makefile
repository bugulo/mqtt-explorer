all: clean build

build:
	qmake -o Makefile.qmake Project.pro
	make -f Makefile.qmake

clean:
	rm -rf mqtt-explorer *.o ui_* moc_* qrc_*.cpp Makefile.qmake .qmake.stash doc/html doc/latex

doxygen:
	doxygen doc/Doxyfile

pack:
	zip -r 1-xslesa01-xbelko02.zip Makefile README.txt icp.pro src examples doc/Doxyfile

run: all
	./mqtt-explorer