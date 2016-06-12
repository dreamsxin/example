/**
 * HomeController
 *
 * @description :: Server-side logic for managing homes
 * @help        :: See http://sailsjs.org/#!/documentation/concepts/Controllers
 */

module.exports = {
	index: function (req, res){
		sails.log.debug('silly日志！');
		return res.view({title:"首页", content:"欢迎使用 Sails"});
	}
};

