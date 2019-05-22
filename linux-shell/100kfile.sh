#!/bin/bash
read -p  "Enter pattern: " pattern
files=$(sudo find ~ -name $pattern -type f -size +100k)

for file in $files
do
    echo $file
done

for file in $files
do 
    echo -e "\e[34m\e[1m$file\e[0m\e[39m"
    read -p "delete or compress (d or c, press any key to skip)? " opt
    if [ "$opt" = "c" ]; then
        sudo zip 'test.zip' $file >> 'test-compress.log'
    elif [ "$opt" = "d" ]; then
        sudo rm -v $file >> 'test-delete.log'
    else
        echo -e "\e[31m\e[1mSkip\e[0m\e[39m ${file}"
    fi
done