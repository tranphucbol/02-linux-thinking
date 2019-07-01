#!/bin/bash
sum=0
while IFS= read -r var
do
        sum=$(expr $var + $sum)
done < $1
echo "$sum"
