# 1. Linux shell

- [1. Linux shell](#1-Linux-shell)
  - [Processing Text](#Processing-Text)
    - [Count the number of lines satisfying a specific pattern in a log file](#Count-the-number-of-lines-satisfying-a-specific-pattern-in-a-log-file)
    - [Calculate KLOC of code C/C++ files in a directory](#Calculate-KLOC-of-code-CC-files-in-a-directory)
  - [System](#System)
    - [Kill multiple processes following a patterns (using awk, grep, xargs)](#Kill-multiple-processes-following-a-patterns-using-awk-grep-xargs)
    - [Kill processes opening a specific port (using netstat, grep...)](#Kill-processes-opening-a-specific-port-using-netstat-grep)
    - [Find files via regular expressions, and remove them](#Find-files-via-regular-expressions-and-remove-them)
    - [List, one at a time, all files larger than 100K in the /home/username directory tree. Give the user the option to delete or compress the file, then proceed to show the next one. Write to a logfile the names of all deleted files and the deletion times](#List-one-at-a-time-all-files-larger-than-100K-in-the-homeusername-directory-tree-Give-the-user-the-option-to-delete-or-compress-the-file-then-proceed-to-show-the-next-one-Write-to-a-logfile-the-names-of-all-deleted-files-and-the-deletion-times)
  - [Shell Scripting](#Shell-Scripting)
  - [Reference](#Reference)

## Processing Text

### Count the number of lines satisfying a specific pattern in a log file

Dùng `grep` để tìm chuỗi pattern chỉ định trong một file, thêm `-c` để đếm số dòng có pattern được nhập. Câu lệnh bên dưới là tìm `error` trong `error.log`

```bash
grep -c 'error' error.log
```

### Calculate KLOC of code C/C++ files in a directory

Dùng lệnh `find` với regex để tìm các files c/c++ trong thư mục hiện hành. Sau đó dùng lệnh cat để in ra các dòng, kết hợp với `wc -l` để đếm số dòng được cat in ra.

```bash
echo $(($(cat $(find . -regextype posix-extended -regex '.*?\.(c(pp)?)$') | wc -l )/1000.0))
```

## System

### Kill multiple processes following a patterns (using awk, grep, xargs)

```bash
kill $(ps -ef | awk '$NF ~ /chrome/ {print $2;}')
```

Khi dùng lệnh `ps -ef` sẽ có kết quả:

```bash
cpu11413  7946  2924  0 13:43 tty2     00:00:01 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  7958  2924  0 13:43 tty2     00:00:01 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  7986  2924  0 13:45 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  8021  2924  0 13:45 tty2     00:00:04 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  8267  2924  1 13:49 tty2     00:00:55 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
root      9491     2  0 14:03 ?        00:00:00 [kworker/1:1]
cpu11413  9494  2924  0 14:03 tty2     00:00:05 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  9507  2924  0 14:03 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  9521  2924  0 14:03 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  9547  2924  0 14:03 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote #--enable-crash-repor
cpu11413  9557  2924 # 1 14:03 tty2     00:00:27 /opt/google/chrome/chrome --type=zygote #--enable-crash-repor
```

Có thể thấy `PID` là nằm ở cột 2 và, tên process nằm ở cột cuối cùng. Dùng lệnh `awk` để tìm ra dòng nào có cột cuối cùng có chứ tên process và in ra cột 2 (PID)

### Kill processes opening a specific port (using netstat, grep...)

```bash
sudo kill $(sudo netstat -pnlt | awk '$4 ~ /:80$/{print substr($7, 1, index($7, "/") - 1)}')
```

Khi dùng lệnh `sudo netstat -plnt` sẽ được kết quả

```bash
Active Internet connections (only servers)
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name
tcp        0      0 127.0.0.1:16392         0.0.0.0:*               LISTEN      11723/code
tcp        0      0 0.0.0.0:8080            0.0.0.0:*               LISTEN      13196/python3
tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN      1427/nginx: master  
tcp        0      0 127.0.0.53:53           0.0.0.0:*               LISTEN      718/systemd-resolve
tcp        0      0 127.0.0.1:631           0.0.0.0:*               LISTEN      967/cupsd
tcp        0      0 127.0.0.1:5432          0.0.0.0:*               LISTEN      1504/postgres
tcp6       0      0 :::45361                :::*                    LISTEN      11723/code
tcp6       0      0 ::1:631                 :::*                    LISTEN      967/cupsd
```

Sẽ thấy `PID/Program name`, nên cần tách `PID` ra, dùng `substr` để cắt chuỗi, và `index` dùng để tìm ra vị trí của `/`.

### Find files via regular expressions, and remove them

`regex` bên dưới dùng để tìm các file `*.c` và `*.cpp` trong thư mục hiện hành.

```bash
rm -rf $(find . -regextype posix-extended -regex '.*?\.(c(pp)?)$')
```

### List, one at a time, all files larger than 100K in the /home/username directory tree. Give the user the option to delete or compress the file, then proceed to show the next one. Write to a logfile the names of all deleted files and the deletion times

```bash
!/bin/bash
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
```

Script đầu tiên sẽ chạy lệnh:

```bash
sudo find ~ -name $pattern -type f -size +100k
```

Để tìm kiếm các file lớn hơn 100KB có trong cây thư mục `/home/username`. Tiếp theo là sẽ kiểm tra điền kiện `opt` để thực hiện lệnh tương ứng.

Chạy file [100kfile.sh](linux-shell/100kfile.sh)

Khi chạy file cần nhập pattern của các file muốn tìm kiếm, và 1 trong 2 option là `compress (c)` và `delete(d)`. Sau đó script sẽ được chạy:

- **compress** thì sẽ có 2 file `test.zip` và `test-compress.log`
- **delete** thì sẽ có file log là `test-delete.log`

## Shell Scripting

```bash
awk '{s+=$1} END {print s}' sample.data
```

Dùng awk để cộng dồn là cách nhanh nhất.

Có thể sử dụng shell script, thì sẽ chậm hơn rất nhiều.

```bash
#!/bin/bash
sum=0
while IFS= read -r var
do
        sum=$(expr $var + $sum)
done < $1
echo "$sum"
```

Dòng while sẽ đọc từng dòng trong file dữ liệu, và cộng dồn vào biến `$sum`.

## Reference

- [Đọc file trong bash script](https://www.shellhacks.com/bash-read-file-line-by-line-while-read-line-loop/)
- [Lệnh grep](https://blogd.net/linux/lenh-grep-toan-tap/)
- [Lệnh xargs](https://blogd.net/linux/lam-quen-voi-xargs/)
- [Lệnh awk](https://blogd.net/linux/su-dung-lenh-awk/)
- [Shell script](https://www.shellscript.sh)