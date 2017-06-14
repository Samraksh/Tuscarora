doxygen standaloneAPI_doxy.conf

pushd standaloneAPI_Docs_Output/latex
make

cp refman.pdf ../../FullAPI_current.pdf

