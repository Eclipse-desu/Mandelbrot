all: blackwhite colored cycle

blackwhite: ./src/blackwhite.cpp ./include/bmp.h
	g++ -o ./bin/blackwhite ./src/blackwhite.cpp

colored: ./src/colored.cpp ./include/bmp.h
	g++ -o ./bin/colored ./src/colored.cpp

cycle: ./src/cycle.cpp ./include/bmp.h
	g++ -o ./bin/cycle ./src/cycle.cpp

report: ./doc/report.tex
# 	cd doc
# 	cd img && ./bmp2png
	cd doc && pwd && xelatex report.tex
	cd doc && bibtex  report
	cd doc && xelatex report.tex
	cd doc && xelatex report.tex
# 	cd ..

.PHONY: clean
clean:
	rm -f ./bin/*
	find ./doc ! -regex ".*\(tex\|bib\)" -type f -exec rm -f {} +