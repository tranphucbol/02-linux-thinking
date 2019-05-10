# 1. Linux shell

## Processing Text

### Count the number of lines satisfying a specific pattern in a log file

Dùng `grep` để tìm chuỗi pattern chỉ định trong một file, thêm `-c` để đếm số dòng có pattern được nhập. Câu lệnh bên dưới là tìm `error` trong `error.log`

```sh
grep -c 'error' error.log
```

### Calculate KLOC of code C/C++ files in a directory

Dùng lệnh `find` với regex để tìm các files c/c++ trong thư mục hiện hành. Sau đó dùng lệnh cat để in ra các dòng, kết hợp với `wc -l` để đếm số dòng được cat in ra.

```sh
echo $(($(cat $(find . -regextype posix-extended -regex '.*?\.(c(pp)?)$') | wc -l )/1000.0))
```

## System

### Kill multiple processes following a patterns (using awk, grep, xargs)

```sh
kill $(ps -ef | awk '$NF ~ /chrome/ {print $2;}')
```

Khi dùng lệnh `ps -ef` sẽ có kết quả:

``` sh
cpu11413  7946  2924  0 13:43 tty2     00:00:01 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  7958  2924  0 13:43 tty2     00:00:01 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  7986  2924  0 13:45 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  8021  2924  0 13:45 tty2     00:00:04 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  8267  2924  1 13:49 tty2     00:00:55 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
root      9491     2  0 14:03 ?        00:00:00 [kworker/1:1]
cpu11413  9494  2924  0 14:03 tty2     00:00:05 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  9507  2924  0 14:03 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  9521  2924  0 14:03 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  9547  2924  0 14:03 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
cpu11413  9557  2924  1 14:03 tty2     00:00:27 /opt/google/chrome/chrome --type=zygote --enable-crash-repor
```

Có thể thấy `PID` là nằm ở cột 2 và, tên process nằm ở cột cuối cùng. Dùng lệnh `awk` để tìm ra dòng nào có cột cuối cùng có chứ tên process và in ra cột 2 (PID)

### Kill processes opening a specific port (using netstat, grep...)

```sh
sudo kill $(sudo netstat -plnt | awk '/8080/{print substr($NF, 1, index($NF, "/") - 1);}')
```

Khi dùng lệnh `sudo netstat -plnt` sẽ được kết quả

``` sh
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

``` sh
rm -rf $(find . -regextype posix-extended -regex '.*?\.(c(pp)?)$')
```

### List, one at a time, all files larger than 100K in the /home/username directory tree. Give the user the option to delete or compress the file, then proceed to show the next one. Write to a logfile the names of all deleted files and the deletion times

``` sh
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
```

Script đầu tiên sẽ chạy lệnh:

``` sh
sudo find ~ -name $pattern -type f -size +100k
```

Để tìm kiếm các file lớn hơn 100KB có trong cây thư mục `/home/username`. Tiếp theo là sẽ kiểm tra điền kiện `opt` để thực hiện lệnh tương ứng.

Chạy file [100kfile.sh](linux-shell/100kfile.sh)

Khi chạy file cần nhập pattern của các file muốn tìm kiếm, và 1 trong 2 option là `compress` và `delete`. Sau đó script sẽ được chạy:

- **compress** thì sẽ có 2 file `test.zip` và `test-compress.log`
- **delete** thì sẽ có file log là `test-delete.log`
