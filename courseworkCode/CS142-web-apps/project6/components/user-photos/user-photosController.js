'use strict';

cs142App.controller('UserPhotosController', ['$scope', '$routeParams', '$resource',
  function($scope, $routeParams, $resource) {
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
