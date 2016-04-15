/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   setting.cpp
 * Author: zhuzx
 * 
 * Created on April 15, 2016, 2:46 PM
 */

#include "setting.h"

void setting::info()
{
	response().out() <<
		"<html>\n"
		"<body>\n"
		"  <h1>setting info</h1>\n"
		"</body>\n"
		"</html>\n";
}

