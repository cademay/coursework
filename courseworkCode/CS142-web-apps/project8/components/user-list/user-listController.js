'use strict';

cs142App.controller('UserListController', ['$scope', '$resource', '$rootScope',
    function ($scope, $resource, $rootScope) {
        $scope.main.title = 'Users';

	var url = "/user/list";
	var UserList= $resource(url);
	
	UserList.query(function(userList) {
		$scope.userList = userList;
		$scope.main.app_context = "User List";
	}, function(error) {
		console.log(error);
	});

	

	$rootScope.$on('LoggedInBroadcast', function() {
		var url = "/user/list";
		var UserList= $resource(url);
		
		UserList.query(function(userList) {
			$scope.userList = userList;
			$scope.main.app_context = "User List";
		}, function(error) {
			console.log(error);
		});
		
	});

	$rootScope.$on('LoggedOutBroadcast', function() {
	
		$scope.userList = [];
		$scope.main.app_context = "Login";
	
	});


	//older version code:
        /*console.log('window.cs142models.userListModel()', window.cs142models.userListModel());*/
	/*$scope.userList = window.cs142models.userListModel()*/

	/*
	function doneCallback(model) {
		$scope.$apply(function () {
			$scope.userList = model;
		});
	}

	$scope.FetchModel(url, doneCallback);
	*/

    }]);

