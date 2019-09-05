'use strict';

cs142App.controller('UserListController', ['$scope',
    function ($scope) {
        $scope.main.title = 'Users';

        /*console.log('window.cs142models.userListModel()', window.cs142models.userListModel());*/
	/*$scope.userList = window.cs142models.userListModel()*/

	var url = "http://localhost:3000/user/list";

	function doneCallback(model) {
		$scope.$apply(function () {
			$scope.userList = model;
			$scope.main.app_context = "User List";
		});
	}

	$scope.FetchModel(url, doneCallback);
	
    }]);

