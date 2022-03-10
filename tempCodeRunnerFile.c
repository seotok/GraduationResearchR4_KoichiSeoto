set terminal emf color "Ryumin-Light-EUC-H" 16
set xlabel '周波数(Hz)'
set ylabel '音圧レベル(dB)'
plot "sin_usiro.txt" u 1:2 with l,"sin_mae.txt" u 1:2 with l,"sin_ue.txt" u 1:2 with l,"sin_sita.txt" u 1:2 with l,