function chkinput_login(form) { //断用户是否输入了用户名
	if (form.fullname.value == "") {
		alert("请输入您的真实姓名！");
		form.fullname.focus();
		return(false);
	}
	if (form.phone.value == "") {
		alert("请输入手机号码！");
		form.phone.focus();
		return(false);
	}
	if (isNaN(form.phone.value)) {
		alert("手机号码只能由数字组成！");
		form.phone.focus();
		return(false);
	}
	if (form.password.value == "") {
		alert("请输入登录密码！");
		form.password.focus();
		return(false);
	}
	if (form.password.value.length < 6 || form.password.value.length > 18) {
		alert("密码必须为6~18个字符！");
		form.password.focus();
		return(false);
	}
	if (form.password1.value == "") {
		alert("请输入确认登录密码！");
		form.password1.focus();
		return(false);
	}
	if (form.password.value != form.password1.value) {
		alert("二次密码输入不相同！");
		form.password1.focus();
		return(false);
	}
	if (form.email.value == "") {
		alert("请输入E-mail地址！");
		form.email.focus();
		return(false);
	}
	if (form.email.value.match(/^(.+)@(.+)$/) == null) {             //判断邮件地址的格式是否正确
		alert("请输入正确的E-mail地址！");
		form.email.focus();
		return(false);
	}
	return(true);
}

