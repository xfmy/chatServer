#! /bin/bash
chmod +x build.sh
cmake . build
startTime=`date +"%Y-%m-%d %H:%M:%S"`
cmake --build build -j1

endTime=`date +"%Y-%m-%d %H:%M:%S"`
st=`date -d  "$startTime" +%s`
et=`date -d  "$endTime" +%s`
sumTime=$(($et-$st))
#echo "Total time is : $sumTime second."
echo "Time taken to build is $sumTime seconds."

rm -r build/*



