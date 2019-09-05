'use strict';

cs142App.controller('UserPhotosController', ['$scope', '$routeParams', '$resource', '$rootScope',
  function($scope, $routeParams, $resource, $rootScope) {
    /*
     * Since the route is specified as '/photos/:userId' in $routeProvider config the
     * $routeParams  should have the userId property set with the path from the URL.
     */

     $scope.userLikesPhoto = function (user_id, photo) {
	console.log("phto:", photo);
	console.log("photo:",photo);
	var photosLikes = photo.likes;
	if (photosLikes === undefined || photosLikes === null || photosLikes.length <= 0) {
		return false;
	}
	
	
	



	if (photosLikes.indexOf(user_id) >= 0) {
		console.log("returning true");
		return true;
	} else {
		return false;
	}
	
     }; 



     var userId = $routeParams.userId;
     var PhotoQuery = $resource('/photosOfUser/:userId', {userId: '@id'});
	
     PhotoQuery.query({userId: userId}, function(photos) {






	var i;
	var p;

	for (i = 0; i < photos.length; i++) {
		p = photos[i];
			
		if ($scope.userLikesPhoto($scope.main.current_user_userId, p)) {
			// if user likes the photo, then the option should be to unlike it
			$scope.like.buttonsTexts[i] = "Unlike";	
		} else {
			// if the user doesn't like the photo yet, then they should be able to like it
			$scope.like.buttonsTexts[i] = "Like";	
		}
			
	
		if (p.likes === undefined || p.likes === null || p.likes.length <= 0) {
			$scope.like.photoLikes[i] = 0;

		} else {
			$scope.like.photoLikes[i] = p.likes.length;
		}
	}	
	



	$scope.currentUserPhotoObjects = photos;



     }, function (error) {
	console.log("photo query failed");
     });
    
     $scope.like = {
	buttonsTexts: [],
	photoLikes: []
     };
 

     $scope.likePhoto = function (photo_id, index_on_page) {


	var PhotoQuery = $resource('/getPhoto/byPhotoID');
	
	PhotoQuery.get({photo_id: photo_id}, function (photoObject) {

		console.log(index_on_page);
		if ($scope.userLikesPhoto($scope.main.current_user_userId, photoObject)) {	

			//dislike photo

			var unlikePOST = $resource('/photo/unlike');

			var feed = {
				photo_id: photo_id,
				user_id: $scope.main.current_user_userId
			};

			unlikePOST.save(feed, function (response) {

		
				$scope.like.buttonsTexts[index_on_page] = "Like";

				$scope.get_likes(photo_id, index_on_page);
				
			}, function (error) {

				console.log("unlike failed");
			});





		} else {
			// like photo
			var likePOST = $resource('/photo/like');
			
			var feed = {
				photo_id: photo_id,
				user_id: $scope.main.current_user_userId
			};

			likePOST.save(feed, function (response) {
				console.log("liked");
	
				$scope.like.buttonsTexts[index_on_page] = "Unlike";

				//$scope.like.photoLikes[index_on_page] = $scope.get_likes(photo_id);
				$scope.get_likes(photo_id, index_on_page);
				//console.log($scope.like);
				
			}, function (error) {

				console.log("like failed");
			});


		}


	}, function (error) {

		console.log("issue liking photo");	



	});



     }; 

     $scope.get_likes = function (photo_id, index_on_page) {

	var PhotoQuery = $resource('/getPhoto/byPhotoID');
	
	PhotoQuery.get({photo_id: photo_id}, function (photo) {
		
		var likes = photo.likes;
		
		if (likes === undefined || likes === null || likes.length <= 0) {
			$scope.like.photoLikes[index_on_page] = 0;
		} else {
			$scope.like.photoLikes[index_on_page] = likes.length;
		}
		console.log("likeadsf:",$scope.like);

	}, function (error) {
		$scope.like.photoLikes[index_on_page] = 0;
	});


     };


     var User = $resource('/user/:userId', {userId: '@id'});
     User.get({userId: userId}, function(user) {	
	$scope.main.app_context = "Photos of " + user.first_name + " " + user.last_name;
     });


     $scope.add_new_comment = {};
     $scope.add_new_comment.currentButton = [];
     $scope.add_new_comment.user_input_text = {};

     $scope.updateDBactivity_comment = function (filename) {
	// update activities list in DB

	var ActivitiesPOST = $resource('/activities/add');
	
	var d = new Date();

	var feed_dict = {
		date: d.toISOString(),
		user_name: $scope.main.current_user_login_name,
		action_string: ' commented on a photo:',
		image_file_name: filename,
		user_id: $scope.main.current_user_userId	
	};

	console.log("dict",feed_dict);

	ActivitiesPOST.save(feed_dict, function (response) {
		console.log('successfully posted');
	}, function (error) {
		console.log('activities post failed');
		console.log(error);
	});



     };

     $scope.add_new_comment.clicked = function(photoObjectId, photoFileName) {
	
	var POSTcommentQuery = $resource('/commentsOfPhoto/:photo_id', {photo_id: '@photo_id'});

	var commIndex = $scope.add_new_comment.currentButton[$scope.add_new_comment.currentButton.length - 1];
	var comm = $scope.add_new_comment.user_input_text[commIndex];

	POSTcommentQuery.save({photo_id: photoObjectId, comment: comm}, function(response) {
		     $scope.add_new_comment.user_input_text[commIndex] = "";
		     var PhotoQuery = $resource('/photosOfUser/:userId', {userId: '@id'});

		     $scope.updateDBactivity_comment(photoFileName); 

		     PhotoQuery.query({userId: response.user_id}, function(photos) {
			$scope.currentUserPhotoObjects = photos;

		     });
	});

     };

     $scope.updateViewWRTdatebase = function () {
	     // queries photos, then sets the current object list equal to the response
             // 2 way binding handles the rest

	     var PhotoQuery = $resource('/photosOfUser/:userId', {userId: '@id'});
		
	     PhotoQuery.query({userId: userId}, function(photos) {
		$scope.currentUserPhotoObjects = photos;
	     });
     };
     
     $rootScope.$on('NewPhotoUploaded', function () {
	$scope.updateViewWRTdatebase();
     });

   
     $scope.usersOwnPhoto = function () {
	
	var currentUserId = $scope.main.current_user_userId;
	var currentPhotoPageId = userId;

	if (currentUserId === currentPhotoPageId) {
		return true;
	} else {
		return false;
	}
     };


     $scope.usersOwnComment = function (currentCommentId) {
	
	var currentUserId = $scope.main.current_user_userId;

	if (currentUserId === currentCommentId) {
		return true;
	} else {
		return false;
	}
     };


     $scope.deletePhoto = function (photo_id) {

	console.log("input id: ",photo_id);
	
	var deleteQuery = $resource('/photos/delete', {photo_id: '@photo_id'});

	deleteQuery.delete({photo_id: photo_id}, function (response) {
		// after response returns from server
		console.log('updating view');	
		$scope.updateViewWRTdatebase();
		
	}, function (error) {
		
		console.log("Delete failed");

	});	

     };

     $scope.deleteComment = function (comment_id, photo_id) {
       	console.log("comment id: ",comment_id);
	console.log("photoId: ", photo_id);

	var deleteQuery = $resource('/comment/delete', {comment_id: '@comment_id', photo_id: '@photo_id'});

	deleteQuery.delete({comment_id: comment_id, photo_id: photo_id}, function (response) {
		// after response returns from server
		$scope.updateViewWRTdatebase();
		
	}, function (error) {
		
		console.log("Delete failed");

	});	


     };


     //older version code:
	/*  console.log('UserPhoto of ', $routeParams.userId);*/
	/*
    console.log('window.cs142models.photoOfUserModel($routeParams.userId)',
       window.cs142models.photoOfUserModel(userId));



    $scope.currentUserPhotoObjects = window.cs142models.photoOfUserModel(userId);
	*/



    /*
    function doneCallback(model) {	
	$scope.$apply(function() { 	
		$scope.currentUserPhotoObjects = model;
	});
     }


    $scope.FetchModel(url, doneCallback);
    */

     /* 
     $scope.FetchModel("/user/" + userId, function (model) {

	$scope.$apply(function () {
		$scope.main.app_context = "Photos of " + model.first_name + " " + model.last_name;
	});

     });
     */
    
  }]);
