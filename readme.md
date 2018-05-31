# HCMC University of Science

# Faculty of Infomation Technology

# Computer Networking

# FTP-Client Project

# Build in Visual Studio with MFC Tech

 ----------------------------------------------------------------------------------------------------
 
## FTP
### 1. Khái niệm

**FTP** là giao thức dùng để truyền file và được xây dựng chuẩn theo tiêu chuẩn TCP. 
 
FTP sử dụng 2 cổng gồm: 
- Cổng 20 để truyền dữ liệu (data port)
- Cổng 21 dùng để truyền câu lệnh (command port) 

Hoạt động theo mô hình Server-Client.

### 2. Mô hình hoạt động

<img src="http://i.imgur.com/M0DK4I4.png" />

Như đã nói ở trên, FTP hoạt động theo mô hình Server-Client.

#### a. Các kênh hoạt động trong FTP

Mặc dù giao thức này sử dụng kết nối TCP, nhưng nó không chỉ dùng một kênh TCP như phần lớn các giao thức truyền thông khác. Mô hình FTP chia quá trình truyền thông giữa bộ phận Server với bộ phận client ra làm hai kênh logic:
- Kênh Điều khiển: Nó dùng để giao tiếp TCP giữa client với Server 
- Kênh Dữ liệu: Kênh này để client và server có thể trao đổi, truyền dữ liệu cho nhau. Khi dữ liệu được truyền xong, kênh này sẽ bị ngắt

#### b. Các tiến trình

<img src="http://www.rhyshaden.com/images/ftp1.gif" />

Do các chức năng điều khiển và dữ liệu sử dụng các kênh khác nhau, nên mô hình hoạt động của FTP cũng chia phần mềm trên mỗi thiết bị ra làm hai thành phần logic tương ứng với mỗi kênh.
- Thành phần Protocol Interpreter (PI) là thành phần quản lý kênh điều khiển, với chức năng phát và nhận lệnh.
- Thành phần Data Transfer Process (DTP) có chức năng gửi và nhận dữ liệu giữa phía client với server. 
- Ngoài ra, cung cấp cho tiến trình bên phía người dùng còn có thêm thành phần thứ ba là giao diện người dùng FTP - thành phần này không có ở phía server.

##### Trên Server

Các tiến trình phía server bao gồm hai giao thức:
- Server Protocol Interpreter (Server-PI): chịu trách nhiệm quản lý kênh điều khiển trên server. Nó lắng nghe yêu cầu kết nối hướng tới từ users trên cổng dành riêng. Khi kết nối đã được thiết lập, nó sẽ nhận lệnh từ phía User-PI, trả lời lại, và quản lý tiến trình truyền dữ liệu trên server.
- Server DataTransfer Process (Server-DTP): làm nhiệm vụ gửi hoặc nhận file từ bộ phận User-DTP. Server-DTP vừa làm nhiệm thiết lập kết nối kênh dữ liệu và lắng nghe một kết nối kênh dữ liệu từ user. Nó tương tác với server file trên hệ thống cục bộ để đọc và chép file.

##### Trên Client

- User Protocol Interpreter (User-PI): chịu trách nhiệm quản lý kênh điều khiển phía client. Nó khởi tạo phiên kết nối FTP bằng việc phát ra yêu cầu tới phía Server-PI. Khi kết nối đã được thiết lập, nó xử lý các lệnh nhận được trên giao diện người dùng, gửi chúng tới Server-PI, và nhận phản hồi trở lại. Nó cũng quản lý tiến trình User-DTP.
- User Data Transfer Process (User-DTP): là bộ phận DTP nằm ở phía người dùng, làm nhiệm vụ gửi hoặc nhận dữ liệu từ Server-DTP. User-DTP có thể thiết lập hoặc lắng nghe yêu cầu kết nối kênh dữ liệu trên server. Nó tương tác với thiết bị lưu trữ file phía client.
- User Interface: cung cấp giao diện xử lý cho người dùng. Nó cho phép sử dụng các lệnh đơn giản hướng người dùng, và cho phép người điều khiển phiên FTP theo dõi được các thông tin và kết quả xảy ra trong tiến trình.

#### c. Thiết lập kênh điều khiển

- Server-PI sẽ lắng nghe các kết nối TCP trên cổng 21
- User-PI sử dụng một cổng bất kỳ (> 1024) để kết nối tới server
- Sau khi thiết lập xong, Server-PI sẽ đáp trả kết quả bằng các mã thông báo
- Sau đó, là bước login của người dùng 

Bước này có hai mục đích:
- Access Control (Điều khiển truy cập): quá trình chứng thực cho phép hạn chế truy cập tới server với những người dùng nhất định. Nó cũng cho phép server điều khiển loại truy cập như thế nào đối với từng người dùng. 
- Resource Selection (Chọn nguồn cung cấp): Bằng việc nhận dạng người dùng tạo kết nối, FTP server có thể đưa ra quyết định sẽ cung cấp những nguồn nào cho người dùng đã được nhận dạng đó.

#### d. Chứng thực

Quy trình chứng thực khá đơn giản

- B1: Client gửi ID và Password từ User-PI lên Server-PI bằng lệnh USER và PASS.
- B2: Server kiểm tra ID và Password, nếu hợp lệ thì server sẽ thiết lập phiên làm việc. Nếu sai quá số lần quy định, phiên sẽ được đóng và ngắt kết nối.

#### e. Bảo mật trong FTP

Tất cả được gửi qua kênh kết nối dưới dạng clear-text (Bao gồm cả ID và Password) vì thế FTP không an toàn trên môi trường Internet, các attacker có thể nghe lén và đọc được toàn bộ nội dung bên trong.

### 3. Hoạt động của FTP

#### a. Chế độ ActiveFTP

<img src="http://i0.wp.com/vnitnews.com/wp-content/uploads/2015/11/tong-quan-ve-giao-thuc-ftp-01.png" />

Ở ActiveFTP, Client lắng nghe các kết nối dữ liệu từ Server qua cổng M, nó sẽ gửi các lệnh qua cổng M tới server cổng đang lắng nghe lệnh (21). Mặc định thì M=N, Server sẽ thiết lập kênh truyền dữ liệu qua cổng 20.

##### b. Chế độ PassiveFTP

<img src="http://i1.wp.com/vnitnews.com/wp-content/uploads/2015/11/tong-quan-ve-giao-thuc-ftp-02.png" />

Trường hợp client ở đằng sau Firewall và không cho phép các kết nối TCP, PassiveFTP được sử dụng trong trường hợp này. Khi đó, client sẽ sử dụng một kết nối điều khiển gửi một câu lệnh PASV tới server yêu cầu port truyền data của server. Sau khi nhận được Port của server, client sẽ sử dụng một cổng random để kết nối dữ liệu vào bằng cổng bất kỳ của mình đến port vừa nhận được.

##### c. Phương thức truyền dữ liệu

Khi kênh dữ liệu đã được thiết lập xong giữa Server-DTP với User-DTP, dữ liệu sẽ được truyền trực tiếp từ phía client tới phía server, hoặc ngược lại, dựa theo các lệnh được sử dụng. Do thông tin điều khiển được gửi đi trên kênh điều khiển, nên toàn bộ kênh dữ liệu có thể được sử dụng để truyền dữ liệu.

**Stream mode:**

- Dữ liệu được truyền đi dưới dạng các byte không cấu trúc liên tiếp
- Bên gửi chỉ đơn thuần đầy luồng dữ liệu qua kết nối TCP tới phía nhận.
- Phương thức này chủ yếu dựa vào tính tin cậy trong truyền dữ liệu của TCP.
- Do không có các header, vì vậy khi truyền file xong kết nối sẽ bị ngắt
- Xử lý với các file đều đơn thuần như là xử lý dòng byte, mà không để ý tới nội dung của các file
- Không tốn dung lượng để thông báo header

**Block mode:**

- Phương thức này mang tính quy chuẩn hơn, dữ liệu được chia thành nhiều khối nhỏ và được đóng gói thành các FTP block
- Mỗi block này có một trường header 3 byte báo hiệu độ dài, và chứa thông tin về các khối dữ liệu đang được gửi.
- Có một thuật toán để kiểm tra các dữ liệu đã được gửi đi và phát hiện, khởi tạo lại đối với một phiên dữ liệu đã bị ngắt trước đó.

**Compressed mode:**

- Phương thức truyền sử dụng một kỹ thuật nén đơn giản,
là "Run-length encoding" để phát hiện và xử lý các đoạn lặp trong dữ liệu được gửi đi để giảm chiều dài của thông điệp
Thông tin đã được nén sẽ được gắn các header và được xử lý như ở Block mode.

#### 4. Tham khảo:

- http://sinhvienit.net/forum/tim-hieu-ve-giao-thuc-ftp.28754.html
- https://en.wikipedia.org/wiki/File_Transfer_Protocol

