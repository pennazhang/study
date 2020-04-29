说明：	本章演示了如何通过QProcess::start（）来调用linux脚本。

注意事项：	
1. 如果在windows下执行，请将test.bat文件拷贝到c:\temp\test.bat，然后再运行本程序。
2. 如果在linux下执行，请将test.sh文件拷贝到/git/temp/test.sh，然后在运行本程序。
3. 经过测试，QProcess::readAllStandardError()和 QProcess::readAllStandardOutput()都是非阻塞函数，且调用前，必须确保该进程仍然处于运行状态。
	if (m_process.state() == QProcess::Running)
	{
		QString strOutput = m_process.readAllStandardError();
		*g_pCout << "stderr: " << strOutput;
	}
	