#! /bin/bash
binname=$1
runlist=$2
des_dir=$3
nrun=$4
run_first=$5
#nlsf=20
if [[ -z $nrun ]]; then
   nrun=`cat $runlist | wc -l`
   run_first=0
else
   if [[ -z $run_first ]]; then
      read -n1 -p "use <source lsf.sh runlist eos_dir nrun run_first>,  Press any key to continue..."
      read -n1 -p "use <source lsf.sh runlist eos_dir nrun run_first>,  Press any key to continue..."
   fi
fi
#step=$((nrun/nlsf))
step=0
if [ $step -eq 0 ]; then
   step=$(((nrun-run_first)/600))
fi
if [ $step -eq 0 ]; then
   step=1
fi

queue="ams1nd"
jobindex=0
name0="sh"
for (( i=$run_first; i<nrun; i=i+step ))
do
   runlast=$((i+step-1))
   if [ $runlast -ge $nrun ]; then
      runlast=$((nrun-1))
   fi

   #hostname=`cat /afs/cern.ch/work/h/huliu/Documents/test/bash/serverlist.txt | head -$((jobindex+1)) | tail -1`

     cd lsf
     RUN=$i

     if [[ -f /eos/user/h/hliu/$des_dir/${RUN}.root ]];then
        cd ..
        continue
     fi

     #if [[ `bjobs -q $queue | grep tr${RUN:0:8} | wc -l` -gt 0 ]];then
     #   cd ..
     #   continue
     #fi


     echo "#! /bin/bash" > $RUN.$name0
     echo "# info" >> $RUN.$name0
     echo "echo hostname \`hostname\` ..." >> $RUN.$name0
     echo "echo pwd \`pwd\`" >> $RUN.$name0
     echo "echo ROOTSYS \$ROOTSYS" >> $RUN.$name0
     echo "echo LD_LIBRARY_PATH \$LD_LIBRARY_PATH" >> $RUN.$name0
     echo $'\n' >> $RUN.$name0
     echo "#set the environment and preparation for compiling and liking" >> $RUN.$name0
     echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> $RUN.$name0
     echo $'\n' >> $RUN.$name0
     echo "#execute the monitor program" >> $RUN.$name0
     echo "time /afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/Noise/$binname $runlist /eos/user/h/hliu/$des_dir/${RUN}.root -1 $i $runlast" >> $RUN.$name0
     echo $'\n' >> $RUN.$name0

     echo "ls -ltrh" >> $RUN.$name0
     echo $'\n' >> $RUN.$name0
     echo "#the end" >> $RUN.$name0

     #echo "if [ -f $RUN.root ]; then" >> clean.sh
     #echo "   rm $RUN.root" >> clean.sh
     #echo "fi" >> clean.sh

     chmod a+x $RUN.$name0

     #njobs=`hep_q -p virtual -u hliu | wc -l`
     #sleep_time=120
     #while [ "$njobs" -gt 350 ]
     #do
     #   echo "there are $((njobs)) jobs,too many,sleep for $sleep_time seconds at `date`"
     #   sleep $sleep_time
     #   njobs=`hep_q -p virtual -u hliu | wc -l`
     #done

     hep_sub -p virtual -g lhaaso -dir /eos/user/h/hliu/jobout -o /eos/user/h/hliu/jobout/$RUN.out -e /eos/user/h/hliu/jobout/$RUN.err $RUN.$name0
     #source $RUN.$name0
     #echo "Processing $i : $RUN.$name0 ..."
     #ssh -o 'StrictHostKeyChecking=no' huliu@$hostname sh /afs/cern.ch/work/h/huliu/Documents/charge/lsf/$RUN.$name0 &

     cd ..

   jobindex=$((jobindex+1))
done

#njobs=`bjobs | grep plot | wc -l`
#sleep_time=120
#while [ "$njobs" -gt 0 ]
#do
#   echo "there are $((njobs)) jobs,too many,sleep for $sleep_time seconds at `date`"
#   sleep $sleep_time
#   njobs=`bjobs | grep plot | wc -l`
#done
#
#cd ~/eos/ams/user/h/huliu/DST/$des_dir
#hadd -f plot.root plot_*.root
#cd ~/Documents/plot
