## MagicSocket 基本架构与知识 ##

  * MagicSocket 类库中所有的名字全部位于 namespace Magic 中。
  * MagicSocket 类库中大部分类都从 Magic::Object 继承，这样继承出来的类拥有引用计数。
  * Magic::Pointer 实现了自动引用计数管理，并且声明了一些更加方便的名字，如 SocketPtr、BufferPtr 等。
  * MagicSocket 类库中抛出的所有异常均为 std::exception 或其子类。
  * Magic::Socket 中的 BeginXXX、EndXXX 为异步函数，其余的函数为同步函数。

## 环境 ##

本类库目前在 Microsoft Visual Studio 2008 下配合 AnkhSvn 进行开发，因此您最好装有这两款软件。

  * Microsoft Visual Studio 2008 是一款 Win32 下的集成开发环境。如果您的工程不需要用到 MFC/ATL 及一些企业版功能，您可以使用 Microsoft Visual Studio 2008 Express，该软件是免费的，可以在这里下载：http://www.microsoft.com/express/ 。您也可以向 Microsoft 购买具有更多功能的收费版本。
  * AnkhSvn 是一个集成在 Visual Studio 中的 SVN 版本控制软件，可以在这里下载：http://ankhsvn.open.collab.net/ 。

下面的步骤均假设您的计算机上已装有这两款软件。

## 创建解决方案 ##

请按照下列步骤操作：

  1. 新建一个 Win32 控制台应用程序，在向导中选中“空项目”。
  1. 文件 - Subversion - Add Project From Subversion，添加 http://magicsocket.googlecode.com/svn/trunk/MagicSocket/MagicSocket/MagicSocket.vcproj ，注意选取合适的存储路径。
  1. 回到刚才我们创建的项目，我们发现新的项目并没有被添加进来，手动添加之：文件 - 添加 - 现有项目，然后选择刚才导出的 MagicSocket.vcproj。
  1. 添加项目依赖：选中我们新建的项目，在项目 - 项目依赖项中选中 MagicSocket。
  1. 添加链接引用：在我们新建的项目的属性页中找到配置属性 - 链接器 - 输入，在文本框中增加 $(OutDir)\MagicSocket.lib。注意不同的配置文件 (如 Debug 或 Release) 的设置是独立的。

## 添加代码 ##

在我们新建的项目中添加一个 C++ 代码文件，并输入下列代码：

```
#include "../MagicSocket/Socket.h"
#include <windows.h>
#include <stdexcept>
using namespace Magic;

void OnReceive(const AsyncResultPtr &asyncResult)
{
	try {
		SocketPtr acceptSocket = asyncResult->asyncState;
		BufferPtr buffer = asyncResult->buffer;
		size_t size = acceptSocket->EndReceive(asyncResult);
		if (size != 0) {
			buffer->Resize(size);
			acceptSocket->BeginSend(buffer);
			acceptSocket->BeginReceive(BufferPtr(8192, NoRebindTag()), &OnReceive, acceptSocket);
		} else {
			acceptSocket->BeginDisconnect();
		}
	} catch (const std::exception &) {
	}
}

void OnAccept(const AsyncResultPtr &asyncResult)
{
	try {
		SocketPtr listenSocket = asyncResult->asyncState;
		SocketPtr acceptSocket = listenSocket->EndAccept(asyncResult);
		acceptSocket->BeginReceive(BufferPtr(8192, NoRebindTag()), &OnReceive, acceptSocket);
		listenSocket->BeginAccept(&OnAccept, listenSocket);
	} catch (const std::exception &) {
	}
}

int main()
{
	try {
		SocketPtr listenSocket;
		listenSocket->Bind(7777);
		listenSocket->Listen();
		listenSocket->BeginAccept(&OnAccept, listenSocket);
		Sleep(INFINITE);
	} catch (const std::exception &) {
	}
}
```

## 生成解决方案 ##

如果没有意外，您可以直接编译成功并成功链接。最可能出现的问题在第一行的包含文件，若出现找不到 MagicSocket.h 文件的情况，请以合适的相对路径改写之。

## 功能测试 ##

上面的 Echo Server 实现了一个 TCP 回应服务器，即服务器将接收到的数据原样发送回来，该服务端监听本地 TCP 端口 7777。您可以使用 telnet 客户端来测试，具体方法如下：

  1. 开始 - 运行，输入命令行 telnet 127.0.0.1 7777。
  1. 随意地敲打一些字符，若无意外，在 telnet 客户端内字符会成对地显示，表明发送的字符与接收到的字符一致。

可能遇到的问题：

  * 某些防火墙会阻止应用程序监听本地网络端口，请参考防火墙软件的使用手册或联系其供应商。
  * 某些较高版本的 Windows (如 Windows 7) 中默认没有安装 telnet 客户端，您需要在“打开/关闭 Windows 功能”中手动安装。
  * 如果您关闭了 telnet 客户端的本地回显功能，您可能只会看到单一的字符，这并不表明功能出现了异常。