
rm -rf output/run_20230925_162900.dat.dir

#root -x -L "StartMonitor.cpp(\"data/run_20230925_162900.dat\")"
#root -x -L "StartMonitor.cpp(\"test.txt\")"run_20230928_133829.dat
#root -x -q -L "StartMonitor.cpp(\"data/run_20230929_172942.dat\")"
root -x -q -L "StartMonitor.cpp()"

#root -x -L "macros/DecodeOffline.cpp(\"run_20230928_124448.dat\")"