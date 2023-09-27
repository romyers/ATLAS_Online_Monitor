
rm -rf output/run_20230925_162900.dat.dir

root -x -L "Monitor.cpp(\"data/run_20230925_162900.dat\")"
#root -x -L "Monitor.cpp(\"test.txt\")"

#root -x -L "macros/DecodeOffline.cpp(\"run_20230925_162900.dat\")"