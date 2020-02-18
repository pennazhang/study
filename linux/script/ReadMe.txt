---------------------------------------------------------------------------------------------------------
unzip

- 将脚本unzip.sh拷贝到某个主目录，运行下面的脚本，可以将该目录(包括子目录)下所有的*.zip文件找出来，并在其所在的目录下进行解压。

- For example：
	cd /git/umixers
	cp /git/study/linux/script/unzip/unzip.sh . 
	find . -name "*.zip" | xargs ./unzip.sh

---------------------------------------------------------------------------------------------------------

clearEmptyFolder

- 删除指定目录下的所有空目录

- For example：删除

	cd /git/study/linux/script/clearEmptyFolder/clearEmptyFolder.sh .
	./clearEmptyFolder.sh /git/umixers

---------------------------------------------------------------------------------------------------------
