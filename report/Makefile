PDFCMD = pdflatex -synctex=1 -interaction=nonstopmode 

all:report.pdf

%.toc:%.tex *.tex
	${PDFCMD} $< --shell-escape

%.aux:%.tex *.tex
	${PDFCMD} $< --shell-escape

%.bbl:%.aux references.bib *.tex
	bibtex $(basename $<)

%.dvi:%.tex *.tex %.bbl
	${DVICMD} $<

%.pdf:%.tex *.tex %.toc %.bbl
	${PDFCMD} $< --shell-escape

clean:
	rm -f *.{toc,bbl,blg,log,pdf,aux,out,synctex.gz,run.xml}
	rm -f report-blx.bib

.PHONY: all clean pdf
.PRECIOUS: %.toc %.bbl %.aux
