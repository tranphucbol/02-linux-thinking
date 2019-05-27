# Docker

<div align="center">
    <img alt="linux" src="images/docker.png.png">
</div>

## Khái niệm về container

Container là một đơn vị phầm mềm tiêu chuẩn đóng gói mã và tất cả các phụ thuộc của nó để ứng dụng chạy nhanh và đáng tin cậy từ môi trường này sang môi trường khác.

Các process trong một conatiner bị cô lập với các process của các contianer khác trong cùng mọt hệ thống tuy nhiên tất cả các container này đều chia sẻ kernel của host OS  (dùng chung host OS). Contianer được đóng gói và setting đầy đủ, có thể tải về, bật nó lên, cho app chạy bên trong môi trường container đó. Khi không sài nữa có thể tắt hoặc xóa bỏ mà không ảnh hưởng gì đến máy.

## Cách hoạt động của Docker

<div align="center">
    <img alt="linux" src="images/server_side_code_flow.png.png">
</div>

Hệ thống Dockere được thực thi với 3 bước chính:

> Build -> Push -> Pull,Run

### Build

Đầu tiên sẽ tạo ra một dockerfile. Dockerfile này sẽ được Build tại mọt máy tính đã cài đặt Docker Engine. Sau khi build ta sẽ thu được Container, trong Container này chứ bộ thư viện và ứng dụng của chúng ta.

### Push

Sau khi có được Container, chúng ta thực hiện push Container này lên đám mây và lưu trữ ở đó. Việc push này có thể thực hiện qua môi trường mạng Internet.

### Pull, Run

Gỉa sử một máy tính muốn sử dụng Containner chúng ta đã push đám mây (máy đã cài Docker Engine) thì bắt buộc máy phải thực hiện Pull container này về máy. Sau đó thực hiện Run Container này.

## Các lệnh cơ bản của Docker

**Pull một iamge từ Docker hub:**

```sh
sudo docker pull image_naem
```

**Tạo mới contianer bằng cách chạy image, kèm các tùy chọn:**

```sh
sudo docker run -v <folder_in_computer>:<folder_in_container> -p \
<port_in_computer>:<port_in_container> -it <image_name> /bin/bash
```

**Liệt kê các images hiện có:**

```sh
docker images
```

**Xóa một images:**

```sh
docker rmi {image_id/name}
```

**Liệt kê các container đang chạy:**

```sh
docker ps
```

**Liệt kê các container đã tắt:**

```sh
docker ps -a
```

**Xóa một container:**

```sh
docker rm -f {container_id/name}
```

**Khởi động một container:**

```sh
docker start {new_container_name}
```

**Truy cập vào container đang chạy:**

```sh
docker exec -it {new_container_name} /bin/bash
```

## Dockerfile cơ bản

`Dockerfile` build một chương trình `C` đơn giản:

```sh
FROM gcc

COPY main.c .

RUN gcc -o main.c main

CMD ["./main"]

```

## Reference

- [Dockerfile (1)](https://viblo.asia/p/docker-chua-biet-gi-den-biet-dung-phan-1-lich-su-ByEZkWrEZQ0)
- [Dockerfile (2)](https://viblo.asia/p/docker-chua-biet-gi-den-biet-dung-phan-2-dockerfile-RQqKLzeOl7z)