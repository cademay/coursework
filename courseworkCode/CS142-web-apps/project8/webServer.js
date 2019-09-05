"use strict";

/* jshint node: true */

/*
 * This builds on the webServer of previous projects in that it exports the current
 * directory via webserver listing on a hard code (see portno below) port. It also
 * establishes a connection to the MongoDB named 'cs142project6'.
 *
 * To start the webserver run the command:
 *    node webServer.js
 *
 * Note that anyone able to connect to localhost:portNo will be able to fetch any file accessible
 * to the current user in the current directory or any of its children.
 *
 * This webServer exports the following URLs:
 * /              -  Returns a text status message.  Good for testing web server running.
 * /test          - (Same as /test/info)
 * /test/info     -  Returns the SchemaInfo object from the database (JSON format).  Good
 *                   for testing database connectivity.
 * /test/counts   -  Returns the population counts of the cs142 collections in the database.
 *                   Format is a JSON object with properties being the collection name and
 *                   the values being the counts.
 *
 * The following URLs need to be changed to fetch there reply values from the database.
 * /user/list     -  Returns an array containing all the User objects from the database.
 *                   (JSON format)
 * /user/:id      -  Returns the User object with the _id of id. (JSON format).
 * /photosOfUser/:id' - Returns an array with all the photos of the User (id). Each photo
 *                      should have all the Comments on the Photo (JSON format)
 *
 */
//https://piazza.com/class/jf8vmv8h8hu1g7?cid=748
var mongoose = require('mongoose');
var async = require('async');


// Load the Mongoose schema for User, Photo, and SchemaInfo
var User = require('./schema/user.js');
var Photo = require('./schema/photo.js');
var SchemaInfo = require('./schema/schemaInfo.js');
var Activity = require('./schema/activity.js');


var express = require('express');
var app = express();

var fs = require('fs');
// project7 spec: Add these to your webServer.js using require statements like:
var session = require('express-session');
var bodyParser = require('body-parser');
var multer = require('multer');
var processFormBody = multer({storage: multer.memoryStorage()}).single('uploadedphoto');



app.use(session({secret: 'secretKey', resave: false, saveUninitialized: false}));
app.use(bodyParser.json());

// XXX - Your submission should work without this line
var cs142models = require('./modelData/photoApp.js').cs142models;

mongoose.connect('mongodb://localhost/cs142project6');

// We have the express static module (http://expressjs.com/en/starter/static-files.html) do all
// the work for us.
app.use(express.static(__dirname));


app.get('/', function (request, response) {
    response.send('Simple web server of files from ' + __dirname);
});

/*
 * Use express to handle argument passing in the URL.  This .get will cause express
 * To accept URLs with /test/<something> and return the something in request.params.p1
 * If implement the get as follows:
 * /test or /test/info - Return the SchemaInfo object of the database in JSON format. This
 *                       is good for testing connectivity with  MongoDB.
 * /test/counts - Return an object with the counts of the different collections in JSON format
 */
app.get('/test/:p1', function (request, response) {
    // Express parses the ":p1" from the URL and returns it in the request.params objects.
    console.log('/test called with param1 = ', request.params.p1);

    var param = request.params.p1 || 'info';

    if (param === 'info') {
        // Fetch the SchemaInfo. There should only one of them. The query of {} will match it.
        SchemaInfo.find({}, function (err, info) {
            if (err) {
                // Query returned an error.  We pass it back to the browser with an Internal Service
                // Error (500) error code.
                console.error('Doing /user/info error:', err);
                response.status(500).send(JSON.stringify(err));
                return;
            }
            if (info.length === 0) {
                // Query didn't return an error but didn't find the SchemaInfo object - This
                // is also an internal error return.
                response.status(500).send('Missing SchemaInfo');
                return;
            }

            // We got the object - return it in JSON format.
            console.log('SchemaInfo', info[0]);
            response.end(JSON.stringify(info[0]));
        });
    } else if (param === 'counts') {
        // In order to return the counts of all the collections we need to do an async
        // call to each collections. That is tricky to do so we use the async package
        // do the work.  We put the collections into array and use async.each to
        // do each .count() query.
        var collections = [
            {name: 'user', collection: User},
            {name: 'photo', collection: Photo},
            {name: 'schemaInfo', collection: SchemaInfo}
        ];
        async.each(collections, function (col, done_callback) {
            col.collection.count({}, function (err, count) {
                col.count = count;
                done_callback(err);
            });
        }, function (err) {
            if (err) {
                response.status(500).send(JSON.stringify(err));
            } else {
                var obj = {};
                for (var i = 0; i < collections.length; i++) {
                    obj[collections[i].name] = collections[i].count;
                }
                response.end(JSON.stringify(obj));

            }
        });
    } else {
        // If we know understand the parameter we return a (Bad Parameter) (400) status.
        response.status(400).send('Bad param ' + param);
    }
});

/*
 * URL /user/list - Return all the User object.
 */
app.get('/user/list', function (request, response) {
	//old code:
	//response.status(200).send(cs142models.userListModel());

    	//new code:
		
	//console.log(request.session.login_name);
	//console.log(request.session.user_id);
	if (request.session.user_id === null || request.session.user_id === undefined) {
		response.status(401).send("Unauthorized");
		//console.log("undef session");
	} else {

		var query = User.find({});
		query.select("_id first_name last_name").exec(doneCallback);

	}
	function doneCallback(error, userList) {
		if (error || userList === [] || userList === null || userList === undefined) {
			response.status(400).send(JSON.stringify(error));	
		} else {
			response.status(200).send(userList);
		}
	}

});

/*
 * URL /user/:id - Return the information for User (id)
 */
app.get('/user/:id', function (request, response) {
    	var id = request.params.id;
    	/*
    	var user = cs142models.userModel(id);
    	if (user === null) {
      	  console.log('User with _id:' + id + ' not found.');
          response.status(400).send('Not found');
          return;
    	}
   	response.status(200).send(user);
        */

	if (request.session.user_id === null || request.session.user_id === undefined) {
		response.status(401).send("Unauthorized");
		return null;
	}

	var query  = User.findOne({_id: id});

	query.select("_id first_name last_name location description occupation").exec(doneCallback);

	function doneCallback(error, user) {	
		if (error) {
			response.status(400).send(JSON.stringify(error));
		} else if (user === null || user === undefined || user === []) {
			response.status(400).send("Could not find user with the given ID.");
		} else {
			response.status(200).send(user);
		}
	}

});

/*
 * URL /photosOfUser/:id - Return the Photos for User (id)
 */
app.get('/photosOfUser/:id', function (request, response) {


	if (request.session.user_id === null || request.session.user_id === undefined) {
		response.status(401).send("Unauthorized");
		return null;
	}

    	var id = request.params.id;
	var photoParams = "_id user_id comments file_name date_time likes";

	var query = Photo.find({user_id: id});	
	query.select(photoParams).exec(findCallback);

	//findCallback for Photo.find ^^ 
	function findCallback(error, photoList) {
		
		if (error) { 
			response.status(400).send(JSON.stringify(error));
		} else {
			var photosByUser = [];

			async.each(photoList, function(photoObject, callback) {
				//iterator, called on each photo

				var photo = JSON.parse(JSON.stringify(photoObject));
				var fixedComments = [];

				//replace user_id in comment object with actual user
				async.each(photo.comments, function(commentObject, callback) {
					
					// iterator, called on each comment
					var idOfCommenter = commentObject.user_id;
					var commentCreatorQuery = User.findOne({_id: idOfCommenter});
					commentCreatorQuery.select("_id first_name last_name").exec(commentCreatorQueryCallback);
				
					//called once the comment creator user object is retrieved
					function commentCreatorQueryCallback(error, commenter) {

						if (error) {
							console.log(error);
						} else {
							commentObject.user = JSON.parse(JSON.stringify(commenter));	
						}

						delete commentObject.user_id;

						callback();
					}

				}, function(error) {
					//main callback for comments: called after iterator is done with every comment
					photosByUser.push(photo);
					callback();
						
				});

			}, function(error) {
				//main callback for photoList. called after iterator is done with every photo
				response.status(200).send(photosByUser);	
			});

		}

	}

    	/*
    var photos = cs142models.photoOfUserModel(id);
    if (photos.length === 0) {
        console.log('Photos for user with _id:' + id + ' not found.');
        response.status(400).send('Not found');
        return;
    }
    response.status(200).send(photos);
    	*/
});

app.post('/admin/login', function(request, response) {

	var user_login_name = request.body.login_name;
	var entered_password = request.body.password;
	var query = User.findOne({login_name: user_login_name});
	
	query.select('_id login_name first_name password').exec(doneCallback);
	
	console.log(entered_password);
	function doneCallback(error, user) {

		if (error) {
			response.status(400).send(JSON.stringify(error));
		} else if (user === null || user === undefined || user === []) {
			response.status(400).send("Could not find user with the given login name.");
		} else {
			console.log("user:", user);
			if (entered_password === user.password) {

				//if user successfully identified and logged in:
				//store user info in session state	
				request.session.user_id = user._id;
				request.session.login_name = user.login_name;
					
				//  reply with information needed by your app for logged in user	
				response.status(200).send(user);

			} else {
				response.status(400).send("incorrect password");
			}
		}
	}



});

app.post('/admin/logout', function(request, response) {


	if (request.session.user_id === null || request.session.user_id === undefined) {
		response.status(400).send("Bad request.");
		return null;
	} else {
		// if someone is logged in, then log them out, destroy session
		

		request.session.destroy(function(error) {
			if (error) {
				response.status(400).send("Bad request.");
			} else {
				response.status(200).send("logout successful");
			}
		});
	}


});



app.post('/commentsOfPhoto/:photo_id', function(request, response) {

	
	var photoId = request.params.photo_id;
	var commentText = request.body.comment;

	if (commentText.length <= 0 || commentText === undefined || commentText === null) {
		response.status(400).send('Bad request. No text entered');
	} else {


		var photoParams = "_id user_id comments file_name date_time";
		var photoQuery = Photo.findOne({_id: photoId});
		photoQuery.select(photoParams).exec(function (error, photoObject) {

			var photo = JSON.parse(JSON.stringify(photoObject));			

			var commentObject = {};
			commentObject.comment = commentText;
			commentObject.user_id = request.session.user_id;
			
			var d = new Date();
			commentObject.date_time = d.toISOString();

			photoObject.comments = photoObject.comments.concat([commentObject]);

			photoObject.save();	
			response.status(200).send(photoObject);
			
	
		});
	}

		
});


app.post('/photos/new', function(request, response) {

	//You will need to create a new Photo in the database (this will be done in the fs.writeFile function)
	//  The unique name, along with the creation data and logged in user id, should be placed in the new Photo object you create.	
	var d = new Date();
	var date = d.toISOString();
	

	processFormBody(request, response, function (err) {
	    if (err || !request.file) {
		// XXX -  Insert error handling code here.
		return;
	    }
	    // request.file has the following properties of interest
	    //      fieldname      - Should be 'uploadedphoto' since that is what we sent
	    //      originalname:  - The name of the file the user uploaded
	    //      mimetype:      - The mimetype of the image (e.g. 'image/jpeg',  'image/png')
	    //      buffer:        - A node Buffer containing the contents of the file
	    //      size:          - The size of the file in bytes

	    // XXX - Do some validation here.
	    // We need to create the file in the directory "images" under an unique name. We make
	    // the original file name unique by adding a unique prefix with a timestamp.
	    var timestamp = new Date().valueOf();
	    var filename = 'U' +  String(timestamp) + request.file.originalname;

	    fs.writeFile("./images/" + filename, request.file.buffer, function (err) {
	      	// XXX - Once you have the file written into your images directory under the name
	     	// filename you can create the Photo object in the database
		var PhotoModel = new Photo({file_name: filename, date_time: date, user_id: request.session.user_id, comments: [], likes:[] });

		PhotoModel.save(function (error) {

			if (error) {
				console.log(error);
			}
	
			response.status(200).send(filename);
		
		});




	    });
	});




	
});



app.post('/user', function(request, response) {
	
	console.log("received:", request.body);
	var first_name = request.body.first_name;
	var last_name = request.body.last_name;
	var loc = request.body.loc;
	var description = request.body.description;
	var occupation = request.body.occupation;
	var login_name = request.body.login_name;
	var password = request.body.password;


	if (login_name.length <= 0) {
		response.status(400).send("Login name must be non-empty.");
	} else if (first_name.length <= 0) {
		response.status(400).send("First name must be non-empty.");
	} else if (last_name.length <= 0) {
		response.status(400).send("Last name must be non-empty.");
	} else if (password.length <= 0) {
		response.status(400).send("Password must be non-empty.");
	} else {


		var query = User.findOne({login_name: login_name});
	
		query.select('_id login_name first_name').exec(function(error, user) {

			if (user === null || user === undefined) {

				// username available, create new user object and save it here

				var feed_dict = {
					first_name: first_name,
					last_name: last_name,
					login_name: login_name,
					location: loc,
					description: description,
					occupation: occupation,
					password: password

				};

				var UserModel = new User(feed_dict);

				UserModel.save(function (error) {

					if (error) {
						console.log(error);
					}
					console.log(21132);
					response.status(200).send("user created.");
				
				});


			} else {
				console.log(34343434);
				response.status(400).send("Username already exists.");
			}
		});
		



	}

});


app.delete('/photos/delete', function (request, response) {

	console.log(request);	
	var photo_id = request.query.photo_id;
	console.log('deleting photo', photo_id);

	Photo.findOne({_id: photo_id}, function (error, photo) {
	
		if (error) {
			response.status(400).send(error);
		} else {

			photo.remove(function (err) {
				if (err) {
					response.status(400).send(err);
				} else {	
					response.status(200).send();
				} 

			
			});
		}
		
	});

});

app.delete('/comment/delete', function (request, response) {

	var photo_id = request.query.photo_id;
	var comment_id = request.query.comment_id;
	console.log('deleting photo', photo_id);

	Photo.findOne({_id: photo_id}, function (error, photoObject) {
	
		if (error) {
			response.status(400).send(error);
		} else {

			var photo = JSON.parse(JSON.stringify(photoObject));
			console.log("b ",photoObject);	
			for (var i = 0; i < photoObject.comments.length; i++) {
				var currComment = photoObject.comments[i];
		
				if (String(currComment._id).trim() === comment_id.trim()) {
					photoObject.comments.splice(i, 1);
				}
			}	
			
			photoObject.save();
			response.status(200).send();	
		}
		
	});

});


app.delete('/user/delete', function (request, response) {

	var user_id = request.query.user_id;
	console.log('idsfasdfasdf', user_id);	

	// find all the comments this user created, delete them
	Photo.find({'comments.user_id': user_id}, function (error, photoObjects) {
		
		if (error) {
			request.status(400).send(error);
		} else {
			// for each photo containing a comment posted by user_id
			for (var i = 0; i < photoObjects.length; i++) {
			
				var photo = JSON.parse(JSON.stringify(photoObjects[i]));
	
				// delete each comment, if user_id created it
				for (var j = 0; j < photo.comments.length; j++) {
		
					var currComment = photoObjects[i].comments[j];
					//delete comment
					console.log("currc:",currComment);
					if (String(currComment.user_id).trim() === user_id.trim()) {
						photoObjects[i].comments.splice(j, 1);
					}
				}	
		
				photoObjects[i].save();
	
			}
		
			// now, delete each photo posted by user_id
			Photo.find({user_id: user_id}, function (error, photos) {
		

				if (error) {
					response.status(400).send(error);
				} else {
					for (var k = 0; k < photos.length; k++) {

						photos[k].remove(function (err) {
							if (err) {
								response.status(400).send(err);
							}
						});
					}

					// delete user's activities
					Activity.find({user_id: user_id}, function (error, activities) {
							
						if (error) {
							response.status(400).send(error);
						} else {

							// delete activities
							for (var m = 0; m < activities.length; m++) {
								console.log("act: ", activities[m]);
								activities[m].remove(function (activityError) {
									
									if (activityError) {
										response.status(400).send(activityError);
									}	
								});
							}

							Photo.find({likes: user_id}, function (error, likedPhotos) {
								
								if (error) {
									response.status(400).send(error);
								} else {

									for (var g = 0; g < likedPhotos.length; g++) {
										
										photo = JSON.parse(JSON.stringify(likedPhotos[g]));

										if (photo.likes === undefined || photo.likes === null) {
											photo.likes = [];
											likedPhotos[g].save();
										} 

										if (photo.likes.indexOf(user_id) >= 0) {

											var index = photo.likes.indexOf(user_id); 
											likedPhotos[g].likes.splice(index, 1);
											likedPhotos[g].save();

										}
								
									}


									// finally, delete the user object with user_id
									User.findOne({_id: user_id}, function (error, user) {

										if (error) {
											response.status(400).send(error);
										} else {
											console.log("userasdfadsfadsf", user);
							
											user.remove(function (userRemoveError) {
												if (userRemoveError) {
													response.status(400).send(userRemoveError);
												} else {
													response.status(200).send();
												}
											});
										}
									});




								}

							});
							
							
	
				



						}

					});


				}

			});

			
		}
	});




});


app.get('/activities/list', function (request, response) {


	var ActivityQuery = Activity.find({});
	ActivityQuery.sort('date').exec(function (error, activitiesList) {

		if (error) { 
			response.status(400).send(error);
		} else {
			console.log("list:");
			for (var i = 0; i < activitiesList.length; i++) {
				console.log(activitiesList[i]);
			}
		}	response.status(200).send(activitiesList);

	});

	/*	
	Activity.find({}, function (error, activitiesList) {

		if (error) { 
			response.status(400).send(error);
		} else {
			console.log("list:");
			for (var i = 0; i < activitiesList.length; i++) {
				console.log(activitiesList[i]);
			}
		}	response.status(200).send(activitiesList);

	});
	*/


});


app.post('/activities/add', function (request, response) {

	var date = request.body.date;
	var user_name = request.body.user_name;
	var action_string = request.body.action_string;
	var image_file_name = request.body.image_file_name;
	var user_id = request.body.user_id;

	var feed_dict = {
		date: date,
		user_name: user_name,
		action_string: action_string,
		image_file_name: image_file_name,
		user_id: user_id
	};


	var ActivityModel = new Activity(feed_dict);

	ActivityModel.save(function (error) {

		if (error) {
			console.log(error);
			
			response.status(400).send(error);
		}
		
		response.status(200).send("activity created.");
	
	});


});

app.post('/photo/like', function (request, response) {

	var photo_id = request.body.photo_id;
	var liker_user_id = request.body.user_id;

	Photo.findOne({_id: photo_id}, function (error, photoObject) {

		if (error) {
			response.status(400).send(error);
		} else {
			
			var photo = JSON.parse(JSON.stringify(photoObject));

			if (photoObject.likes === undefined || photoObject.likes === null) {
				photoObject.likes = [];
			} 

			photoObject.likes = photoObject.likes.concat([liker_user_id]);

			photoObject.save();

				
			response.status(200).send(photoObject);
		}

	});



});


app.post('/photo/unlike', function (request, response) {

	var photo_id = request.body.photo_id;
	var liker_user_id = request.body.user_id;

	var photoParams = "_id user_id comments file_name date_time likes";


	var query = Photo.findOne({_id: photo_id});

	query.select(photoParams).exec(callback);

	function callback(error, photoObject) {

		if (error) {
			response.status(400).send(error);
		} else {
		
			console.log("about to remove comm");	
			var photo = JSON.parse(JSON.stringify(photoObject));

			if (photoObject.likes === undefined || photoObject.likes === null) {
				photoObject.likes = [];
				photoObject.save();
			} 

			
			if (photoObject.likes.indexOf(liker_user_id) >= 0) {

				var index = photoObject.likes.indexOf(liker_user_id); 
				photoObject.likes.splice(index, 1);
				console.log();	
				photoObject.save();

			}
				

			response.status(200).send(photoObject);

		}

	}

});

app.get('/getPhoto/byPhotoID', function (request, response) {

	var photo_id = request.query.photo_id;

	Photo.findOne({_id: photo_id}, function (error, photoObject) {
		
		if (error) {
			response.status(400).send(error);
		} else {
			console.log("obj::",photoObject);
			response.status(200).send(JSON.parse(JSON.stringify(photoObject)));
		}
	})

});

var server = app.listen(3000, function () {
    var port = server.address().port;
    console.log('Listening at http://localhost:' + port + ' exporting the directory ' + __dirname);
});


