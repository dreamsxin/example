/**
 * HomeController
 *
 * @description :: Server-side logic for managing homes
 * @help        :: See http://sailsjs.org/#!/documentation/concepts/Controllers
 */

module.exports = {
	index: function (req, res){
		return res.view("home/index",{title:"首页", content:"欢迎使用 Sails"});
	}
};

