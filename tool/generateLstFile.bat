
::生成bin hex lst文件
set fileName=c9_project
fromelf --bin .\Objects\%fileName%.axf --output ..\..\debug\%fileName%.bin
fromelf --i32 .\Objects\%fileName%.axf --output ..\..\debug\%fileName%.hex
fromelf --text -c .\Objects\%fileName%.axf  1>..\..\debug\%fileName%.lst

::拷贝文件
copy .\Objects\%fileName%.axf ..\..\debug /y


::fromelf --bin .\Objects\c9_project.axf --output ..\..\debug\c9_project.bin
::fromelf --i32 .\Objects\c9_project.axf --output ..\..\debug\c9_project.hex
::fromelf --text -c .\Objects\c9_project.axf  1>..\..\debug\c9_project.lst
