(function () {
  'use strict';
  var Foxx = require('org/arangodb/foxx'),
    Joi = require('joi'),
    Model;

  Model = Foxx.Model.extend({
    schema: {
      // Describe the attributes with Joi here
      '_key': Joi.string(),
	  'name': Joi.string(),
    }
  });

  exports.Model = Model;
}());
