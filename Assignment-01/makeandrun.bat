GOTO CompileAndRun

:CompileAndRun
	gcc -std=gnu99 -D_XOPEN_SOURCE=500 -Wall megamaniac.c -o megamaniac -I../ZDK04 -L../ZDK04 -lzdk04 -lncurses -lm
	C:\cygwin64\bin\mintty.exe ./megamaniac
	pause
	GOTO CompileAndRun