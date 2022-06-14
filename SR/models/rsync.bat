
@echo off

 scp.exe -r -o ConnectTimeout=2 lz@116.56.143.182:~/data/lz_project/models/* ./

if %errorlevel%  == 0 (
	echo successful
) else (
	echo failed
)
