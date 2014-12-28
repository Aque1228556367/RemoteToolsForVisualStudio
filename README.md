RemoteToolsForVisualStudio
==========================

尝试做一个在VS中可以远程打开Linux终端的插件

最终目标：在VS中可以远程打开Linux终端（Visual GDB 的只能外挂，不能和VS其他窗口整合）
最后是要把putty（开源的）整合到自己创建的工具栏窗口中
但是需要先看懂putty，是在哪一步怎么创建终端窗口的

MSDN 资料：
http://msdn.microsoft.com/zh-cn/library/envdte80.windows2.createtoolwindow2.aspx
http://msdn.microsoft.com/zh-cn/library/dwtd01y4.aspx

RemoteBuild_src
CSDN 上一个作者的代码，经过整理已经能实现自动上传代码到指定的Linux主机路径

尝试用 C++/ATL 的方式开发供 Visual Studio 使用的外接程序。
增加一个 MS 官方提供的例程供参考
源地址：http://www.microsoft.com/en-us/download/details.aspx?id=9122
