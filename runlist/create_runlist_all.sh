#! /bin/bash
dir=$1
itel=$2
itel=`printf "%02d" $itel`
if [ -f all_tel${itel}.txt ];then
   rm all_tel${itel}.txt
fi

for ith in `ls -l $dir | grep ^d | awk '{print $9}'`
do
length=`echo $ith | wc -L`
if [ $length -ne 4 ];then
   continue
fi

echo $ith
for ith2 in `ls $dir/$ith | grep .event.root$ | grep WFCTA${itel}`
do
#echo $dir/$ith/$ith2

   filename=$dir/$ith/$ith2
   filesize=`ls $filename -l | awk '{print $5}'`
   if [ $filesize -lt 10000 ];then
   continue
   fi

echo $dir/$ith/$ith2 >>all_tel${itel}.txt
done

done
