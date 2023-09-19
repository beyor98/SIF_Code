
【非常重要】
如果你需要在GreeIDE中编译rabbit时，请按照以下步骤操作：

1，为rabbit目录新建父目录eclipse_rabbit，效果是这样的： 
	 eclipse_rabbit/rabbit
	 
2，将eclipse_rabbit/rabbit/tool/mbed/.mbed文件拷贝到eclipse_rabbit中，效果是这样的：
 	 eclipse_rabbit/rabbit
 	 eclipse_rabbit/.mbed
 	
3，打开Dos窗口，cd到eclipse_rabbit目录下执行以下命令：
	 ./rabbit/tool/mbed/mbed.exe export -i GreeIDE -m GM6611
	 
4，打开GreeIDE后，导入eclipse_rabbit工程即可。

如有任何问题，请毫不犹豫的找芯片所550239寻求帮助，谢谢！