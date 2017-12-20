> #文件分享系统

### 客户端
> code: -dl+code 下载   -ul+文件路径 上传   -del+code  删除  -dp 显示文件列表
> 1. 客户端在控制台找中通过输入不同的可选项执行不同的操作
> 2. 请求服务器的时候通过不同的请求码标识不同的请求，采取不同的操作
> 3. 上传：fopen(pname, "rw") ==> （文件名发过去，服务端存的时候要带上用户的标识）把文件读出来，然后发到
> 4. 删除：就直接传code
> 5. 下载：传code接收文件
> 6. 显示文件列表：服务器调用opendir("directory path")。客户端接收数据流显示出来


* 上传文件
* 显示文件列表
* 删除文件（仅文件的上传者拥有删除权限）
  * remove(filename)
* 下载文件

### 服务器端
    将客户端传过来的文件存在一个固定的文件夹下。