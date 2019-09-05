'use strict';

var cs142App = angular.module('cs142App', ['ngRoute', 'ngMaterial']);

cs142App.config(['$routeProvider',
    function ($routeProvider) {
        $routeProvider.
            when('/users', {
                templateUrl: 'components/user-list/user-listTemplate.html',
                controller: 'UserListController'
            }).
            when('/users/:userId', {
                templateUrl: 'components/user-detail/user-detailTemplate.html',
                controller: 'UserDetailController'
            }).
            when('/photos/:userId', {
                templateUrl: 'components/user-photos/user-photosTemplate.html',
                controller: 'UserPhotosController'
            }).
            otherwise({
                redirectTo: '/users'
            });
    }]);

cs142App.controller('MainController', ['$scope',
    function ($scope) {
        $scope.main = {};
        $scope.main.title = 'Users';

     /*
      * FetchModel - Fetch a model from the web server.
      *   url - string - The URL to issue the GET request.
      *   doneCallback - function - called with argument (model) when the
      *                  the GET request is done. The argument model is the
      *                  objectcontaining the model. model is undefined in
      *                  the error case.
      */
     $scope.FetchModel = function(url, doneCallback) {


	var xhr = new XMLHttpRequest();
	xhr.onreadystatechange = function xhrHandler() {

	
		if (this.readyState !== 4) {
			return;
		}

		if (this.status !== 200) {
			return;
		}

		var object = JSON.parse(this.responseText);
		doneCallback(object);
	};







	xhr.open("GET", url);
	xhr.send();

     };


     var versionInfoURL = "/test/info";

     function doneCallback(model) {
	$scope.$apply(function () {
		$scope.versionInfoModel = model;
	});
     }

     $scope.FetchModel(versionInfoURL, doneCallback);


    }]);




  
