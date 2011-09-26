#sh scripts/source_code.sh | xargs enscript -T3 -2r  -C -pformatted.ps -Ecpp
cat files | xargs enscript -T3 -f Courier9  -C -pformatted.ps -Ecpp
mpage -2 formatted.ps > formatted2.ps
pstopdf formatted2.ps
