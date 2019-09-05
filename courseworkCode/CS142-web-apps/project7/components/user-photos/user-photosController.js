'use strict';

cs142App.controller('UserPhotosController', ['$scope', '$routeParams', '$resource', '$rootScope',
  function($scope, $routeParams, $resource, $rootScope) {
    /*
     * Since the route is specified as '/photos/:userId' in $routeProvider config the
     * $routeParams  should have the userId property set with the path from the URL.
     */
     var userId = $routeParams.userId;
     var PhotoQuery = $resource('/photosOfUser/:userId', {userId: '@id'});
	
     PhotoQuery.query({userId: userId}, function(photos) {
	$scope.currentUserPhotoObjects = photos;
     });
     
     
     var User = $resource('/user/:userId', {userId: '@id'});
     User.get({userId: userId}, function(user) {	
	$scope.main.app_context = "Photos of " + user.first_name + " " + user.last_name;
     });


     $scope.add_new_comment = {};
     $scope.add_new_comment.currentButton = [];
     $scope.add_new_comment.user_input_text = {};

     $scope.add_new_comment.clicked = function(photoObjectId, modelIndex) {
	
	var POSTcommentQuery = $resource('/commentsOfPhoto/:photo_id', {photo_id: '@photo_id'});

	var commIndex = $scope.add_new_comment.currentButton[$scope.add_new_comment.currentButton.length - 1];
	var comm = $scope.add_new_comment.user_input_text[commIndex];

	POSTcommentQuery.save({photo_id: photoObjectId, comment: comm}, function(response) {
		     $scope.add_new_comment.user_input_text[commIndex] = "";
		     var PhotoQuery = $resource('/photosOfUser/:userId', {userId: '@id'});
		     PhotoQuery.query({userId: response.user_id}, function(photos) {
			$scope.currentUserPhotoObjects = photos;
		     });
	});

     };
     
     $rootScope.$on('NewPhotoUploaded', function () {
	
	     var PhotoQuery = $resource('/photosOfUser/:userId', {userId: '@id'});
		
	     PhotoQuery.query({userId: userId}, function(photos) {
		$scope.currentUserPhotoObjects = photos;
	     });

     });

   
     $scope.usersOwnPhoto = function () {
	
	var currentUserId = $scope.main.current_user_userId;
	var currentPhotoPageId = userId;

	if (currentUserId === currentPhotoPageId) {
		return true;
	} else {
		return false;
	}

     }

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
