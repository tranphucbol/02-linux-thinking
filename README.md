# Linux thinking

- [Linux thinking](#linux-thinking)
  - [Linux shell](#linux-shell)
    - [Processing Text](#processing-text)
      - [Count the number of lines satisfying a specific pattern in a log file](#count-the-number-of-lines-satisfying-a-specific-pattern-in-a-log-file)
      - [Calculate KLOC of code C/C++ files in a directory](#calculate-kloc-of-code-cc-files-in-a-directory)
    - [System](#system)
      - [Kill multiple processes following a patterns (using awk, grep, xargs)](#kill-multiple-processes-following-a-patterns-using-awk-grep-xargs)
      - [Kill processes opening a specific port (using netstat, grep...)](#kill-processes-opening-a-specific-port-using-netstat-grep)
      - [Find files via regular expressions, and remove them](#find-files-via-regular-expressions-and-remove-them)
      - [List, one at a time, all files larger than 100K in the /home/username directory tree. Give the user the option to delete or compress the file, then proceed to show the next one. Write to a logfile the names of all deleted files and the deletion times](#list-one-at-a-time-all-files-larger-than-100k-in-the-homeusername-directory-tree-give-the-user-the-option-to-delete-or-compress-the-file-then-proceed-to-show-the-next-one-write-to-a-logfile-the-names-of-all-deleted-files-and-the-deletion-times)
    - [Shell Scripting](#shell-scripting)
  - [Linux System Programming](#linux-system-programming)
    - [File và File System](#file-v%C3%A0-file-system)
      - [File descriptor là gì?](#file-descriptor-l%C3%A0-g%C3%AC)
      - [Dạng tập tin](#d%E1%BA%A1ng-t%E1%BA%ADp-tin)
        - [Regular file](#regular-file)
        - [Special file](#special-file)
          - [Các tập tin thiết bị](#c%C3%A1c-t%E1%BA%ADp-tin-thi%E1%BA%BFt-b%E1%BB%8B)
          - [Các ống có tên (pipes)](#c%C3%A1c-%E1%BB%91ng-c%C3%B3-t%C3%AAn-pipes)
          - [Các socket](#c%C3%A1c-socket)
          - [Liên kết mềm](#li%C3%AAn-k%E1%BA%BFt-m%E1%BB%81m)
          - [Stdin, stdout and Stderr](#stdin-stdout-and-stderr)
  - [Reference](#reference)

## Linux shell

### Processing Text

#### Count the number of lines satisfying a specific pattern in a log file

Dùng `grep` để tìm chuỗi pattern chỉ định trong một file, thêm `-c` để đếm số dòng có pattern được nhập. Câu lệnh bên dưới là tìm `error` trong `error.log`

```sh
grep -c 'error' error.log
```

#### Calculate KLOC of code C/C++ files in a directory

Dùng lệnh `find` với regex để tìm các files c/c++ trong thư mục hiện hành. Sau đó dùng lệnh cat để in ra các dòng, kết hợp với `wc -l` để đếm số dòng được cat in ra.

```sh
echo $(($(cat $(find . -regextype posix-extended -regex '.*?\.(c(pp)?)$') | wc -l )/1000.0))
```

### System

#### Kill multiple processes following a patterns (using awk, grep, xargs)

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
cpu11413  9547  2924  0 14:03 tty2     00:00:00 /opt/google/chrome/chrome --type=zygote #--enable-crash-repor
cpu11413  9557  2924 # 1 14:03 tty2     00:00:27 /opt/google/chrome/chrome --type=zygote #--enable-crash-repor
```#

Có thể thấy `PID` là nằm ở cột 2 và, tên process nằm ở cột cuối cùng. Dùng lệnh `awk` để tìm ra dòng nào có cột cuối cùng có chứ tên process và in ra cột 2 (PID)

#### Kill processes opening a specific port (using netstat, grep...)

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

#### Find files via regular expressions, and remove them

`regex` bên dưới dùng để tìm các file `*.c` và `*.cpp` trong thư mục hiện hành.

``` sh
rm -rf $(find . -regextype posix-extended -regex '.*?\.(c(pp)?)$')
```

#### List, one at a time, all files larger than 100K in the /home/username directory tree. Give the user the option to delete or compress the file, then proceed to show the next one. Write to a logfile the names of all deleted files and the deletion times

``` sh
##!/bin/bash
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

### Shell Scripting

``` sh
##!/bin/bash
sum=0
while IFS= read -r var
do
        sum=$(expr $var + $sum)
done < $1
echo "$sum"
```

Dòng while sẽ đọc từng dòng trong file dữ liệu, và cộng dồn vào biến `$sum`.

## Linux System Programming

### File và File System

#### File descriptor là gì?

Một **file discription** là số định duy nhất một tệp đang mở trong hệ điều hành. Nó mô tả một tài nguyên dữ liệu và cách tài nguyên đó có thể truy cập được.

Discriptor được định danh bởi một số nguyên không âm duy nhất.

![Abstrations](images/file-descriptors.png)

#### Dạng tập tin

##### Regular file

Một **regular file** là một loại tệp có thể được lưu trữ trong hệ thống tệp. Hầu hết files được sử dùng trực tiếp bởi người dùng là các tệp thông thường. Ví dụ: tệp thực thi, tệp văn bản, tệp hình ảnh cũng là tệp thường.

##### Special file

Đối với hệ điều hành thì tập tin chỉ là một chuỗi các type liên tục. Nhờ vậy có thể dùng khái niệm tập tin cho các thiết bị và các đối tượng khác. Điều này đơn giản hóa sự tổ chức và trao đổi các dữ liệu, vì có thể thực hiện ghi dữ liệu vào tập tin, chuyển đữ liệu trên các thiết bị và trao đổi đữ liệu giữa các tiến trình tương tụ nhau. Những thành phần cũng được Linux coi là tập tin.

- Các tập tin thiết bị
- Các ông (kênh) có tên (named pipe)
- Các socket
- Các liên kết mềm (symlinks)

###### Các tập tin thiết bị

Đối với Linux thì tất cả các thiết bị kết nối với máy tính (ổ cứng, ổ tháo rời, terminal, máy in, máy scan, bàn phím, chuột, v.v...) đề là các tập tin. Ví dụ, nếu cần đưa ra màn hình terminal thứ nhất thông tin nào đó, thì hệ thống thực hiện thao tác ghi vào tập tin `/dev/tty1`.

Có hai dạng thiết bị: __ký tự__ (hay còn gọi là các thiết bị trao đổi byte) và **khối** (trao đổi theo khối). Sự khác nhau giữa hai dạng này nằm ở cách đọc và ghi thông tin vào các thiết bị. Các thiết bị ký tự trao đổi thông tin theo từng ký tự trong chế độ chuỗi các byte. Ví dụ thiết bị dạng này là terminal. Còn thông tin được đọc và ghi vào các thiết bị khối theo các khối. ví dụ các ổ cứng. Không thể độc từ đĩa cứng và ghi lên đó từng byte, trao đổi thông tin với đĩa chỉ có thể theo từng khối.

Mỗi dạng thiết bị có thể có một vài tập tin thiết bị. Vì thế các tập tin thiết bị thường có hai số: lớn (major) và nhỏ (minor). Số lớn của thiết bị cho kernel biết là tập tin này thuộc về driver nào, còn số nhỏ cho biết cần phải làm việc với thiết bị cụ thể nào của dạng này. Đối với các tập tin thiết bị, câu lệnh `ls -l` cho biết số lớn và số nhỏ đã nói thay vì kích thước của tập tin.

``` sh
crw-rw-rw-  1 root     tty       5,   0 Thg 5 13 08:38 tty
crw--w----  1 root     tty       4,   0 Thg 5 13 08:38 tty0
crw--w----  1 gdm      tty       4,   1 Thg 5 13 08:38 tty1
crw--w----  1 root     tty       4,  10 Thg 5 13 08:38 tty10
crw--w----  1 root     tty       4,  11 Thg 5 13 08:38 tty11
```

Có thể thấy được số nhỏ và số lớn ở cột 4 và cột 5.

###### Các ống có tên (pipes)

Các ống có tên, hay bộ đệm FIFO (**F**irst **I**n - **F**irst **O**ut). Tập tin dạng này chủ yếu dùng để tổ chức trao đổi dữ liệu giữa các chương trình khác nhau.

Pipe là cách rất hữu dụng để trao đổi thông tin giữa các tiến trình. Một tiến trình có thể đọc tất cả những gì mà một tiến trình khác đặt vào ống. Nếu có hai tiến trình được sinh ra từ một tiến trình mẹ trao đổi thông tin, thì ống có thể không có tên. Trong trường hợp ngược lại cần tạo ra một ống có tên, ví dụ bằng chương tình mkfifo. Khi này bản thân tập tin pipe chỉ tham gia vào sự khởi đầu trao đổi dữ liệu.

###### Các socket

Socket đó là kết nối giữa các tiến trình, cho phép chúng giao tiếp mà không chịu ảnh hưởng của các tiến tình khác. Socket là khái niệm then chốt của TCP/IP và như vậy là dựa trên socket đã xây dựng toàn bộ Internet. Từ phía hệ thống tập tin socket thực tế không khác các pipe: đó chỉ là các điểm cho phép nối các chương trình với nhau. Sau khi tạo ra kết nối, thì trao đổi được thực hiện mà không cần đến socket: dữ liệu do kernel chuyển trực tiếp từ chương trình này đến chương trình khác.

Mặc dù những tiến trình khác có thể thấy các tập tin socket, nhưng những tiến trình không tham gia vào kết nối hiện thời không thể thực hiện bất kỳ thao tác đọc hay ghi nào lên tập tin socket.

###### Liên kết mềm

Trong Linux, có dạng liên kết là liên kết cứng. Liên kết cứng chỉ là một tên khác cho tập tin ban đầu. Nó được ghi trong mô tả inode của tập tin đó. Sau khi tạo liên kết cứng khong thể phân biệt đâu là tên tập tin còn đâu là liên kết. Nếu đọc xóa một trong só những tập tin này, thì tập tin vẫn được lưu trên đĩa cho đến khi còn ít nhất một liên kết cứng. Điểm đặc biệt của liên kết cứng là nó chỉ thẳng đến chỉ số inode, và do đó liên kết cứng chỉ có thể đùng cho tập tin của một hệ thống tập tin, tức là trên cùng một phân vùng.
Nhưng trên Linux có một dạng liên kết khác gọi là (liên kết tượng trưng). Những liên kết này cũng có thể coi là tên phục cho tập tin, nhưng chúng là những tập tin khác - những tập tin liên kết mềm. Khác với liên kết cứng, liên kết mềm có thể chỉ đến những tập tin nằm trong hệ thống tập tin khác, ví dụ trên những đĩa lưu động, hoặc thậm chí trên một máy tính khác. Nếu tập tin đầu bị xóa thì liên kết này trở nên vô giá trị.

###### Stdin, stdout and Stderr

|Tên |File discriptor   |Mô tả   |
|---|---|---|
|Standard input (stdin)  |0   |Luồng dữ liệu mặc định cho đầu vào, ví dụ trong một command pipelin. Trong terminal, mặc định bàn phím là đầu vào từ user   |
|Standard output (stdout)  |1   |Luồng dữ liệu mặc định cho đầu ra, ví dụ khi một lệnh in ra đoạn text. Trong terminal, mặc định là màn hình của user   |
|Standard error (stderr)   |2   |Luồng dữ liệu mặc định cho đầu ra có xảy ra lỗi. Trong terminal, mặc định là màn hình của user   |

## Reference

- [Đọc file trong bash script](https://www.shellhacks.com/bash-read-file-line-by-line-while-read-line-loop/)
- [Lệnh grep](https://blogd.net/linux/lenh-grep-toan-tap/)
- [Lệnh xargs](https://blogd.net/linux/lam-quen-voi-xargs/)
- [Lệnh awk](https://blogd.net/linux/su-dung-lenh-awk/)
- [Shell script](https://www.shellscript.sh)
- [File discriptors (1)](https://www.bottomupcs.com/file_descriptors.xhtml)
- [File discriptors (2)](https://www.computerhope.com/jargon/f/file-descriptor.htm)
- [Regular file](https://www.computerhope.com/jargon/r/regular-file.htm)
- [Special file](https://www.computerhope.com/jargon/s/special-file.htm)