'use strict';

cs142App.controller('UserListController', ['$scope', '$resource',
    function ($scope, $resource) {
        $scope.main.title = 'Users';

	var url = "http://localhost:3000/user/list";
	var UserList= $resource(url);
	
	UserList.query(function(userList){
		$scope.userList = userList;
		$scope.main.app_context = "User List";
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

