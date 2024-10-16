# Supervisor

## 安装

```shell
sudo apt-get install supervisor
```


## 使用

将每个进程的配置文件单独拆分放在`/etc/supervisor/conf.d/`目录下，例如`test.conf`定义的一个简单的 HTTP 服务器：

```conf
[program:test]
command=python -m SimpleHTTPServer
```

`supervisor` 重新加载配置文件，然后启动`test`进程：
```
supervisorctl reload
supervisorctl start test
```

## 常用命令

```shell
supervisorctl start php
supervisorctl restart php
supervisorctl stop program:
supervisorctl stop all
supervisorctl reload
supervisorctl update
```
日志文件 `/var/log/supervisor/supervisord.log`

注意：显式用stop停止掉的进程，用reload或者update都不会自动重启

## 配置

```conf
[program:php]					; 分组名以及进程名
command=php test.php			; 被监控的进程路径
numprocs=1						; 启动几个进程
directory=/user/local/bin		; 执行前需要进入的目录
autostart=true					; 随着 Supervisord 的启动而启动
autorestart=true				; 自动重启
startretries=10					; 启动失败时的最多重试次数
exitcodes=0						; 正常退出代码
stopsignal=KILL					; 用来杀死进程的信号
stopwaitsecs=10					; 发送 SIGKILL 前的等待时间
redirect_stderr=true			; 重定向 stderr 到 stdout
stdout_logfile=logfile			; 指定日志文件
```

更多说明

```conf
; Sample supervisor config file.
;
; For more information on the config file, please see:
; http://supervisord.org/configuration.html
;
; Note: shell expansion ("~" or "$HOME") is not supported.  Environment
; variables can be expanded using this syntax: "%(ENV_HOME)s".
 
[unix_http_server]          ; supervisord的unix socket服务配置
file=/tmp/supervisor.sock   ; socket文件的保存目录
;chmod=0700                 ; socket的文件权限 (default 0700)
;chown=nobody:nogroup       ; socket的拥有者和组名
;username=user              ; 默认不需要登陆用户 (open server)
;password=123               ; 默认不需要登陆密码 (open server)
 
;[inet_http_server]         ; supervisord的tcp服务配置
;port=127.0.0.1:9001        ; tcp端口
;username=user              ; tcp登陆用户
;password=123               ; tcp登陆密码
 
[supervisord]                ; supervisord的主进程配置
logfile=/tmp/supervisord.log ; 主要的进程日志配置
logfile_maxbytes=50MB        ; 最大日志体积，默认50MB
logfile_backups=10           ; 日志文件备份数目，默认10
loglevel=info                ; 日志级别，默认info; 还有:debug,warn,trace
pidfile=/tmp/supervisord.pid ; supervisord的pidfile文件
nodaemon=false               ; 是否以守护进程的方式启动
minfds=1024                  ; 最小的有效文件描述符，默认1024
minprocs=200                 ; 最小的有效进程描述符，默认200
;umask=022                   ; 进程文件的umask，默认200
;user=chrism                 ; 默认为当前用户，如果为root则必填
;identifier=supervisor       ; supervisord的表示符, 默认时'supervisor'
;directory=/tmp              ; 默认不cd到当前目录
;nocleanup=true              ; 不在启动的时候清除临时文件，默认false
;childlogdir=/tmp            ; ('AUTO' child log dir, default $TEMP)
;environment=KEY=value       ; 初始键值对传递给进程
;strip_ansi=false            ; (strip ansi escape codes in logs; def. false)
 
; the below section must remain in the config file for RPC
; (supervisorctl/web interface) to work, additional interfaces may be
; added by defining them in separate rpcinterface: sections
[rpcinterface:supervisor]
supervisor.rpcinterface_factory = supervisor.rpcinterface:make_main_rpcinterface
 
[supervisorctl]
serverurl=unix:///tmp/supervisor.sock ; use a unix:// URL  for a unix socket
;serverurl=http://127.0.0.1:9001 ; use an http:// url to specify an inet socket
;username=chris              ; 如果设置应该与http_username相同
;password=123                ; 如果设置应该与http_password相同
;prompt=mysupervisor         ; 命令行提示符，默认"supervisor"
;history_file=~/.sc_history  ; 命令行历史纪录
 
; The below sample program section shows all possible program subsection values,
; create one or more 'real' program: sections to be able to control them under
; supervisor.
 
;[program:theprogramname]
;command=/bin/cat              ; 运行的程序 (相对使用PATH路径, 可以使用参数)
;process_name=%(program_name)s ; 进程名表达式，默认为%(program_name)s
;numprocs=1                    ; 默认启动的进程数目，默认为1
;directory=/tmp                ; 在运行前cwd到指定的目录，默认不执行cmd
;umask=022                     ; 进程umask，默认None
;priority=999                  ; 程序运行的优先级，默认999
;autostart=true                ; 默认随supervisord自动启动，默认true
;autorestart=unexpected        ; whether/when to restart (default: unexpected)
;startsecs=1                   ; number of secs prog must stay running (def. 1)
;startretries=3                ; max # of serial start failures (default 3)
;exitcodes=0,2                 ; 期望的退出码，默认0,2
;stopsignal=QUIT               ; 杀死进程的信号，默认TERM
;stopwaitsecs=10               ; max num secs to wait b4 SIGKILL (default 10)
;stopasgroup=false             ; 向unix进程组发送停止信号，默认false
;killasgroup=false             ; 向unix进程组发送SIGKILL信号，默认false
;user=chrism                   ; 为运行程序的unix帐号设置setuid
;redirect_stderr=true          ; 将标准错误重定向到标准输出，默认false
;stdout_logfile=/a/path        ; 标准输出的文件路径NONE＝none;默认AUTO
;stdout_logfile_maxbytes=1MB   ; max # logfile bytes b4 rotation (default 50MB)
;stdout_logfile_backups=10     ; # of stdout logfile backups (default 10)
;stdout_capture_maxbytes=1MB   ; number of bytes in 'capturemode' (default 0)
;stdout_events_enabled=false   ; emit events on stdout writes (default false)
;stderr_logfile=/a/path        ; stderr log path, NONE for none; default AUTO
;stderr_logfile_maxbytes=1MB   ; max # logfile bytes b4 rotation (default 50MB)
;stderr_logfile_backups=10     ; # of stderr logfile backups (default 10)
;stderr_capture_maxbytes=1MB   ; number of bytes in 'capturemode' (default 0)
;stderr_events_enabled=false   ; emit events on stderr writes (default false)
;environment=A=1,B=2           ; process environment additions (def no adds)
;serverurl=AUTO                ; override serverurl computation (childutils)
 
; The below sample eventlistener section shows all possible
; eventlistener subsection values, create one or more 'real'
; eventlistener: sections to be able to handle event notifications
; sent by supervisor.
 
;[eventlistener:theeventlistenername]
;command=/bin/eventlistener    ; 运行的程序 (相对使用PATH路径, 可以使用参数)
;process_name=%(program_name)s ; 进程名表达式，默认为%(program_name)s
;numprocs=1                    ; 默认启动的进程数目，默认为1
;events=EVENT                  ; event notif. types to subscribe to (req'd)
;buffer_size=10                ; 事件缓冲区队列大小，默认10
;directory=/tmp                ; 在运行前cwd到指定的目录，默认不执行cmd
;umask=022                     ; 进程umask，默认None
;priority=-1                   ; 程序运行的优先级，默认-1
;autostart=true                ; 默认随supervisord自动启动，默认true
;autorestart=unexpected        ; whether/when to restart (default: unexpected)
;startsecs=1                   ; number of secs prog must stay running (def. 1)
;startretries=3                ; max # of serial start failures (default 3)
;exitcodes=0,2                 ; 期望的退出码，默认0,2
;stopsignal=QUIT               ; 杀死进程的信号，默认TERM
;stopwaitsecs=10               ; max num secs to wait b4 SIGKILL (default 10)
;stopasgroup=false             ; 向unix进程组发送停止信号，默认false
;killasgroup=false             ; 向unix进程组发送SIGKILL信号，默认false
;user=chrism                   ; setuid to this UNIX account to run the program
;redirect_stderr=true          ; redirect proc stderr to stdout (default false)
;stdout_logfile=/a/path        ; stdout log path, NONE for none; default AUTO
;stdout_logfile_maxbytes=1MB   ; max # logfile bytes b4 rotation (default 50MB)
;stdout_logfile_backups=10     ; # of stdout logfile backups (default 10)
;stdout_events_enabled=false   ; emit events on stdout writes (default false)
;stderr_logfile=/a/path        ; stderr log path, NONE for none; default AUTO
;stderr_logfile_maxbytes=1MB   ; max # logfile bytes b4 rotation (default 50MB)
;stderr_logfile_backups        ; # of stderr logfile backups (default 10)
;stderr_events_enabled=false   ; emit events on stderr writes (default false)
;environment=A=1,B=2           ; process environment additions
;serverurl=AUTO                ; override serverurl computation (childutils)
 
; The below sample group section shows all possible group values,
; create one or more 'real' group: sections to create "heterogeneous"
; process groups.
 
;[group:thegroupname]
;programs=progname1,progname2  ; 任何在[program:x]中定义的x
;priority=999                  ; 程序运行的优先级，默认999
 
; The [include] section can just contain the "files" setting.  This
; setting can list multiple files (separated by whitespace or
; newlines).  It can also contain wildcards.  The filenames are
; interpreted as relative to this file.  Included files *cannot*
; include files themselves.
 
;[include]
;files = relative/directory/*.ini
```