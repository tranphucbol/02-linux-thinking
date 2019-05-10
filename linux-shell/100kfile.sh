#!/bin/bash
read -p  "Enter pattern: " pattern
read -p "Enter option: " opt
files=$(sudo find ~ -name $pattern -type f -size +100k)
if [ "$opt" = "compress" ]; then
        sudo zip 'test.zip' $files >> 'test-compress.log'
elif [ "$opt" = "delete" ]; then
        sudo rm -v $files > 'test-delete.log'
else
        echo "Not found option: $opt"
fi