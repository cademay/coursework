'use strict';

cs142App.controller('UserDetailController', ['$scope', '$routeParams', '$resource',
  function ($scope, $routeParams, $resource) {
    /*
     * Since the route is specified as '/users/:userId' in $routeProvider config the
     * $routeParams  should have the userId property set with the path from the URL.
     */
    var userId = $routeParams.userId;
    var User = $resource('/user/:userId', {userId: '@id'});

    User.get({userId: userId}).$promise.then(function(user){
	$scope.userObject = user;
	$scope.main.app_context = "User Detail of " + user.first_name + " " + user.last_name;
    });

    $scope.currentUserOwnsThisPage = function () {

	var currentUserId = $scope.main.current_user_userId;
	var currentPageId = userId;

	if (currentUserId === currentPageId) {
		return true;
	} else {
		return false;
	}
    }

    $scope.deleteAccountButton = {
	text: "Delete Account",
	clickCount: 0
    };

    $scope.deleteAccountButton.clicked = function () {
	
	$scope.deleteAccountButton.clickCount += 1;

	if ($scope.deleteAccountButton.clickCount === 1) {
		$scope.deleteAccountButton.text = "Are you sure? Click again to delete your account."
	} else {
		// after 2 clicks

		console.log("deleting account");

		var user_id = $scope.main.current_user_userId;

		var deleteQuery = $resource('/user/delete', {user_id: '@user_id'});
		deleteQuery.delete({user_id: user_id}, function (response) {
			
			console.log('user deleted');
			$scope.main.logout();	
		}, function (error) {
			
			console.log("delete failed");
		
		});
	}

    };
    // old codecope.usersOwnPhoto = function () {
	
    /*console.log('UserDetail of ', userId);*/

    /*console.log('window.cs142models.userModel($routeParams.userId)', window.cs142models.userModel(userId));*/

    /*$scope.userObject = window.cs142models.userModel(userId);*/


    /*
    function doneCallback(model) { 
	$scope.$apply(function () { 
		$scope.userObject = model;
		$scope.main.app_context = "User Detail of " + model.first_name + " " + model.last_name;
	});
    }


    $scope.FetchModel(url, doneCallback); 
    */

  }]);
