/**
 * User.js
 *
 * @description :: TODO: You might write a short summary of how this model works and what it represents here.
 * @docs        :: http://sailsjs.org/documentation/concepts/models-and-orm/models
 */

module.exports = {
	tableName: 'users',
	autoCreatedAt: 'created',
	autoUpdatedAt: 'updated',
	attributes: {
		username: {
			type: 'string',
			primaryKey: true,
			unique: true,
			maxLength: 32
		},
		email: {
			type: 'string',
			required: true,
			maxLength: 128
		},
		password: {
			type: 'string',
			required: true
		},
	}
};

