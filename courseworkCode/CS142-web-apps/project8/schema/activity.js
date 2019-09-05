
"use strict";
/*
 *  Defined the Mongoose Schema and return a Model for a User
 */
/* jshint node: true */

var mongoose = require('mongoose');

// create a schema
var activitySchema = new mongoose.Schema({
	date: {type: Date, default: Date.now},
	user_name: String,
	action_string: String,
	image_file_name: String,
	user_id: mongoose.Schema.Types.ObjectId
});

// the schema is useless so far
// we need to create a model using it
var Activity = mongoose.model('Activity', activitySchema);

// make this available to our users in our Node applications
module.exports = Activity;
