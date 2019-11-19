#! /bin/bash
dir=$1
imonth=$2
imonth=`printf "%02d" $imonth`
iday=$3
iday=`printf "%02d" $iday`
idate=`date -d "2019-$imonth-$iday 12:00:00" +%s`
idate2=$((idate+86400))
itel=$4
itel=`printf "%02d" $itel`

if [[ -f ${imonth}${iday}_tel${itel}.txt ]];then
   rm ${imonth}${iday}_tel${itel}.txt
fi

inum=0
for ith in `ls -l $dir | grep ^d | awk '{print $9}'`
do
length=`echo $ith | wc -L`
if [[ $length -ne 4 ]];then
continue
fi

month=${ith:0:2}
day=${ith:2:2}
#if [ ! $month -ge 10 ];then
#   continue
#fi
date1=`date -d "2019-$month-$day 00:00:00" +%s`
date2=`date -d "2019-$month-$day 23:59:00" +%s`
if [ $date1 -gt $idate ];then
time1=$date1
else
time1=$idate
fi
if [ $date2 -gt $idate2 ];then
time2=$idate2
else
time2=$date2
fi
if [ $time1 -ge $time2 ];then
continue
fi
#echo $time1"  "$time2

echo $inum"  "$month"  "$day":  "$ith

for ith2 in `ls $dir/$ith | grep .event.root$ | grep WFCTA$itel`
do
   #echo $dir/$ith/$ith2
   hour=${ith2:30:2}
   if [ $idate -gt $date1 ];then
      if [ $hour -lt 12 ];then
         continue
      fi
   else
      if [ $hour -gt 12 ];then
         continue
      fi
   fi
   #echo $hour
   
   filename=$dir/$ith/$ith2
   filesize=`ls $filename -l | awk '{print $5}'`
   if [ $filesize -lt 10000 ];then
   continue
   fi
   
   echo $dir/$ith/$ith2 >> ${imonth}${iday}_tel${itel}.txt
done

inum=$((inum+1))

done
