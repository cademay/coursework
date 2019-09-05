'use strict';

cs142App.controller('UserPhotosController', ['$scope', '$routeParams',
  function($scope, $routeParams) {
    /*
     * Since the route is specified as '/photos/:userId' in $routeProvider config the
     * $routeParams  should have the userId property set with the path from the URL.
     */
    var userId = $routeParams.userId;
  
	/*  console.log('UserPhoto of ', $routeParams.userId);*/
	/*
    console.log('window.cs142models.photoOfUserModel($routeParams.userId)',
       window.cs142models.photoOfUserModel(userId));



    $scope.currentUserPhotoObjects = window.cs142models.photoOfUserModel(userId);
	*/

    var url = "/photosOfUser/" + userId;

    function doneCallback(model) {	
	$scope.$apply(function() { 	
		$scope.currentUserPhotoObjects = model;
	});
     }


    $scope.FetchModel(url, doneCallback);


     $scope.FetchModel("/user/" + userId, function (model) {

	$scope.$apply(function () {
		$scope.main.app_context = "Photos of " + model.first_name + " " + model.last_name;
	});

     });
    
  }]);
