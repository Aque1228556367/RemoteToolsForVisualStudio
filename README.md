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

VSMediaPlayer2.zip
CodeProject 上的一个例子，但是只能在 VS2005/VS2008 下运行
