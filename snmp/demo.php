<?php
/**
	# sudo apt-get install snmpd
	# sudo apt-get install snmp php5-snmp snmp-mibs-downloader

	# snmpwalk -v 2c -c public localhost
	# snmpwalk -v 2c -c public 192.168.1.108 |grep HOST-RESOURCES-MIB::hrProcessorLoad

	# snmptranslate .1.3.6.1.2.1.1.1.0
	# snmptranslate -On SNMPv2-MIB::system.sysUpTime.0
 */
// $session = new SNMP(SNMP::VERSION_3, "192.168.1.108", "authPrivUser");
// $session->setSecurity ( "authPriv",  "SHA" , "12345678", "DES", "87654321");
$session = new SNMP(SNMP::VERSION_3, "localhost", "authOnlyUser");
$session->setSecurity ( "authNoPriv",  "MD5" ,  "12345678");

$ret = $session->get( "SNMPv2-MIB::sysDescr.0" );		// 获取系统基本信息
var_dump(__LINE__, $ret);
//$ret = $session->get( ".1.3.6.1.2.1.1.1.0" );
//var_dump(__LINE__, $ret);
$ret = $session->get( "SNMPv2-MIB::system.sysUpTime.0" );	// 监控时间
var_dump(__LINE__, $ret);
//$ret = $session->get( ".1.3.6.1.2.1.1.3.0" );
//var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.2.1.1.4.0" );			// 系统联系人
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.2.1.1.5.0" );			// 获取机器名
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.2.1.1.6.0" );			// 机器坐在位置
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.2.1.1.7.0" );			// 机器提供的服务
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.2.1.25.1.6.0" );		// 系统运行的进程数
var_dump(__LINE__, $ret);
$ret = $session->walk(".1.3.6.1.2.1.25.4.2.1.2");		// 系统运行的进程列表
var_dump(__LINE__, $ret);
$ret = $session->get( "HOST-RESOURCES-MIB::hrSWRunName.1" );	// 系统运行的进程
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.2.1.2.1.0" );			// 网络接口的数目
var_dump(__LINE__, $ret);
$ret = $session->walk(".1.3.6.1.2.1.2.2.1.2");			// 网络接口信息描述
var_dump(__LINE__, $ret);
$ret = $session->get( "IF-MIB::ifDescr.1" );
var_dump(__LINE__, $ret);
$ret = $session->get( "IF-MIB::ifType.1" );				// 网络接口类型
var_dump(__LINE__, $ret);
$ret = $session->walk(".1.3.6.1.2.1.2.2.1.5");			// 宽带
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.4.1.2021.11.9.0" );		// 用户CPU百分比
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.4.1.2021.11.10.0" );	// 系统CPU百分比
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.4.1.2021.11.11.0" );	// 空闲CPU百分比
var_dump(__LINE__, $ret);
$ret = $session->get( "UCD-SNMP-MIB::laLoad.1" );		// CPU 每1分钟负载平均值
var_dump(__LINE__, $ret);
$ret = $session->get( "HOST-RESOURCES-MIB::hrProcessorLoad.768" );		// CPU的当前负载
var_dump(__LINE__, $ret);
$ret = $session->get( ".1.3.6.1.2.1.25.2.2.0" );		// 总内存
var_dump(__LINE__, $ret);
$ret = $session->get( "UCD-SNMP-MIB::memTotalReal.0" );	// 总内存
var_dump(__LINE__, $ret);
$ret = $session->get( "UCD-SNMP-MIB::memAvailReal.0" );	// 未被使用的内存
var_dump(__LINE__, $ret);
$ret = $session->walk( ".1.3.6.1.2.1.25.2.3" );	// 磁盘使用情况 
var_dump(__LINE__, $ret);
$ret = $session->walk( ".1.3.6.1.2.1.6.13.1.3" );	// 所有 TCP 连接
var_dump(__LINE__, $ret);