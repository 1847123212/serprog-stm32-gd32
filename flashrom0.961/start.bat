@ECHO OFF&PUSHD %~DP0 &TITLE flashrom
mode con cols=100 lines=30
color E
set /p com=     �����봮�ںţ�com ?�����س���
:Menu
Cls
@ echo.
@ echo.���������� �� �� ѡ ��
@ echo.
@ echo.       ��ȡFLASHоƬ�ͺ���Ϣ 	       �� ����1
@ echo.
@ echo.  	    ��ȡFLASH                  �� ����2
@ echo.
@ echo.             ����FLASH    	       �� ����3
@ echo.
@ echo.             д��FLASH                  �� ����4
@ echo.
@ echo.               У��                     �� ����5
@ echo.
@ echo. https://www.flashrom.org/
@ echo. https://github.com/posystorage/serprog-stm32-gd32
@ echo. https://github.com/dword1511/serprog-stm32vcp
@ echo.

set /p input=     ��������ѡ��ѡ����س���
if /i "%input%"=="1" Goto one
if /i "%input%"=="2" Goto two
if /i "%input%"=="3" Goto three
if /i "%input%"=="4" Goto four
if /i "%input%"=="5" Goto five
: one
 %~dp0flashrom -p serprog:dev=\\.\com%com%:115200
pause 
goto Menu
: two
 :suba
@ echo.�˴���Ҫ�Ƚ���һ���ļ���������Ϊxxx.bin�����ϵ��˴�
 set /p road=�϶���Ҫ���浽���ļ�·����
 set road=%road:"=%
 for %%a in ("%road%") do set rn=%%~xa
 set rn=%rn:.=%
 if  %rn%==bin goto oka
 cls
 echo �ļ����� ��������
 pause
 goto suba
 :oka
 set /p chipa=������FLASHоƬ�ͺţ�eg:MX25L3206E�������س���
 %~dp0flashrom -p serprog:dev=\\.\com%com%:115200 -c  %chipa% -r %road%
pause 
goto Menu
: three
 set /p chipb=������FLASHоƬ�ͺţ�eg:MX25L3206E�������س���
 %~dp0flashrom -p serprog:dev=\\.\com%com%:115200 -c %chipb% -E
pause 
goto Menu
: four
 :subb
 set /p road=�϶���Ҫд����ļ����˴���
 set road=%road:"=%
 for %%a in ("%road%") do set rn=%%~xa
 set rn=%rn:.=%
 if  %rn%==bin goto okb
 cls
 echo �ļ����� ��������
 pause
 goto subb
 :okb
 set /p chipc=������FLASHоƬ�ͺţ�eg:MX25L3206E�������س���
 %~dp0flashrom -p serprog:dev=\\.\com%com%:115200 -c %chipc% -w %road%
pause 
goto Menu
: five
 :subc
 set /p road=�϶���ҪУ����ļ����˴���
 set road=%road:"=%
 for %%a in ("%road%") do set rn=%%~xa
 set rn=%rn:.=%
 if  %rn%==bin goto okc
 cls
 echo �ļ����� ��������
 pause
 goto subc
 :okc
 set /p chipd=������FLASHоƬ�ͺţ�eg:MX25L3206E�������س���
 %~dp0flashrom -p serprog:dev=\\.\com%com%:115200 -c %chipd% -v %road%
pause 
goto Menu
