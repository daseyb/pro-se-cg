%
% LLNCS DOCUMENT CLASS -- version 2.10
% for LaTeX2e
%
\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{ACGSeminar}[2012/02/15]

\typeout{***********************************************************************}
\typeout{* Seminar Current Topics in Computer Graphics and Geometry Processing *}
\typeout{***********************************************************************}

\LoadClass[twoside,a4paper,10pt]{article}

\RequirePackage[utf8]{inputenc}
\RequirePackage{graphicx}
\RequirePackage[T1]{fontenc}

\renewcommand\section{\@startsection{section}{1}{\z@}%
                       {-18\p@ \@plus -4\p@ \@minus -4\p@}%
                       {12\p@ \@plus 4\p@ \@minus 4\p@}%
                       {\normalfont\large\bfseries\boldmath
                        \rightskip=\z@ \@plus 8em\pretolerance=10000 }}
\renewcommand\subsection{\@startsection{subsection}{2}{\z@}%
                       {-18\p@ \@plus -4\p@ \@minus -4\p@}%
                       {8\p@ \@plus 4\p@ \@minus 4\p@}%
                       {\normalfont\normalsize\bfseries\boldmath
                        \rightskip=\z@ \@plus 8em\pretolerance=10000 }}
\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
                       {-18\p@ \@plus -4\p@ \@minus -4\p@}%
                       {-0.5em \@plus -0.22em \@minus -0.1em}%
                       {\normalfont\normalsize\bfseries\boldmath}}
\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
                       {-12\p@ \@plus -4\p@ \@minus -4\p@}%
                       {-0.5em \@plus -0.22em \@minus -0.1em}%
                       {\normalfont\normalsize\itshape}}

\RequirePackage{multicol}
\RequirePackage{ae}
\RequirePackage{eurosym}
\RequirePackage{amsmath}
\RequirePackage{amsfonts}
\RequirePackage{amssymb}
\RequirePackage{amsxtra}
\RequirePackage{epsfig}
\RequirePackage{tikz}
\usetikzlibrary{calc,positioning,shapes,shadows,arrows,backgrounds}
\RequirePackage{subfigure}
\RequirePackage{algorithm}
\RequirePackage{algorithmic}
\RequirePackage{textcomp}
\RequirePackage{amsthm}
\RequirePackage{amssymb}
\RequirePackage{dsfont}
\RequirePackage{fancyhdr}
\RequirePackage{mathtools}
\RequirePackage{tabularx}
\RequirePackage[backend=bibtex,style=alphabetic,sorting=nty,hyperref=auto]{biblatex}
%\AtBeginDocument{\RequirePackage[colorlinks=true,citecolor=black,linkcolor=black,urlcolor=black]{hyperref}}
\RequirePackage[font=small,margin=2.5cm,labelfont=bf]{caption}

\let\footnotesize\small

\pagestyle{empty}
%
\widowpenalty=10000
\clubpenalty=10000

\setlength\textheight{242mm}
\setlength\textwidth{160mm}
\setlength\oddsidemargin{0mm}
\setlength\evensidemargin{0mm}
\setlength\unitlength{1mm}
\setlength\topmargin{-15mm}

\AtBeginDocument{
\def\C{\mathds{C}}%
\def\R{\mathds{R}}%
\def\N{\mathds{N}}%
}

\def\keywords#1{\vskip5mm\noindent{\small\bf Keywords }{\small{\it #1}}\par}

\renewenvironment{figure}
               {\setcounter{subfigure}{0}
                \@float{figure}}
               {\end@float}
\renewenvironment{table}
               {\setcounter{subtable}{0}
                \@float{table}}
               {\end@float}


\renewenvironment{abstract}
	{\noindent{\large\bf\abstractname} 
	\vskip10pt%
	\noindent\it}%
	{\par}

%\renewcommand\maketitle{%
%	\thispagestyle{empty}
%  \begin{flushleft}%
% 		\noindent
%    {\Large \bf \@title \par}%
%    \vskip 5mm%
%    \noindent
%    {\Large von\ \@author \par}%
%  \end{flushleft}\par
%  \setcounter{footnote}{0}%
%  \global\let\maketitle\relax
%  \global\let\@author\@empty
%  \global\let\@date\@empty
%  \global\let\@title\@empty
%  \global\let\title\relax
%  \global\let\author\relax
%  \global\let\date\relax
%  \global\let\and\relax%
%  %\tableofcontents
%  \newpage
%}
