#!/bin/bash

texnam=soturon.tex

rm ${texnam%.*}.dvi
platex ${texnam}
dvipdfmx ${texnam%.*}.dvi -o ${texnam%.*}.pdf

### 作成したPDFを開く ###
#evince ${texnam%.*}.pdf &
