pushd APIDoc
doxygen UserAPI_doxy.conf
cp DoxyOutput/myrefman.tex DoxyOutput/latex/

echo "Copying api docs..."
xargs -I{}  rm  "UserManual/apis/"{} < apiList.txt
xargs -I{}  cp "APIDoc/DoxyOutput/latex/"{} "./UserManual/apis/" < apiList.txt 

popd
echo "Done Generating API Docs"
echo "Building User Manaul ..."

pushd UserManual
pdflatex TuscUserManual.tex
bibtex TuscUserManual
pdflatex TuscUserManual.tex
bibtex TuscUserManual
pdflatex TuscUserManual.tex

cp TuscUserManual.pdf ../
popd

echo "Done Generating User Manual"


