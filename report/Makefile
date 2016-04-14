PDFCMD = pdflatex -synctex=1 -interaction=nonstopmode

all:report.pdf

%.toc:%.tex *.tex
	${PDFCMD} $<

%.aux:%.tex *.tex
	${PDFCMD} $<

%.bbl:%.aux references.bib *.tex
	bibtex $(basename $<)

%.dvi:%.tex *.tex %.bbl
	${DVICMD} $<

%.pdf:%.tex *.tex %.toc %.bbl
	${PDFCMD} $<

clean:
	bash -c 'rm -f *.{toc,bbl,blg,log,pdf,aux,out,synctex.gz,run.xml}'
	rm -f report-blx.bib

.PHONY: all clean pdf
.PRECIOUS: %.toc %.bbl %.aux
