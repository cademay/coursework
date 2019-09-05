'use strict';

var cs142App = angular.module('cs142App', ['ngRoute', 'ngMaterial', 'ngResource']);

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
	    when('/login-register', {
		templateUrl: 'components/login-register/login-registerTemplate.html',
		controller: 'LoginRegisterController'
	    }).
	    when('/activities-view', {
		templateUrl: 'components/activities-view/activities-viewTemplate.html',
		controller: 'ActivitiesViewController'
	    }).
            otherwise({
                redirectTo: '/users'
            });
    }]);


/*
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
*/



cs142App.controller('MainController', ['$scope', '$resource', '$rootScope', '$location', '$http',
    function ($scope, $resource, $rootScope, $location, $http) {
        $scope.main = {};
        $scope.main.title = 'Users';
	$scope.main.userLoggedIn = false; 
	$scope.current_user_first_name = "";
	$scope.main.current_user_userId = "";
	$scope.main.current_user_login_name = "";	
	
    	var versionInfoURL = "/test/info";  
    	var Info = $resource(versionInfoURL);
     
     	Info.get(function(info) {
		$scope.versionInfoModel = info;
    	});

	$scope.main.logout_clicked = function () {


		// update activities list in DB

		var ActivitiesPOST = $resource('/activities/add');
		
		var d = new Date();

		var feed_dict = {
			date: d.toISOString(),
			user_name: $scope.main.current_user_login_name,
			action_string: ' has logged out.',
			image_file_name: "",
			user_id: $scope.main.current_user_userId	
		};

		console.log("dict",feed_dict);

		ActivitiesPOST.save(feed_dict, function (response) {
			console.log('successfully posted');
		}, function (error) {
			console.log('activities post failed');
			console.log(error);
		});


		$scope.main.logout();

	
	};



	$rootScope.$on( "$routeChangeStart", function(event, next, current) {
    		if ($scope.main.userLoggedIn === false) {
        		// no logged user, redirect to /login-register unless already there
        		if (next.templateUrl !== "components/login-register/login-registerTemplate.html") {
            			$location.path("/login-register");
        		}
       		}
	});
	
	$scope.main.activitiesViewSegue = function () {
		$location.path("/activities-view");
	};

	$scope.main.updateActivity_photo = function (filename) {

		// update activities list in DB
		console.log("file:",filename.data);
		var ActivitiesPOST = $resource('/activities/add');
		
		var d = new Date();

		var feed_dict = {
			date: d.toISOString(),
			user_name: $scope.main.current_user_login_name,
			action_string: ' uploaded a photo: ',
			image_file_name: filename.data,
			user_id: $scope.main.current_user_userId	
		};

		ActivitiesPOST.save(feed_dict, function (response) {
			console.log('successfully posted');
		}, function (error) {
			console.log('activities post failed');
			console.log(error);
		});

	};


	var selectedPhotoFile;   // Holds the last file selected by the user

	// Called on file selection - we simply save a reference to the file in selectedPhotoFile
	$scope.inputFileNameChanged = function (element) {
	    selectedPhotoFile = element.files[0];
	};

	// Has the user selected a file?
	$scope.inputFileNameSelected = function () {
	    return !!selectedPhotoFile;
	};

	// Upload the photo file selected by the user using a post request to the URL /photos/new
	$scope.uploadPhoto = function () {
	    if (!$scope.inputFileNameSelected()) {
		console.error("uploadPhoto called will no selected file");
		return;
	    }
	    console.log('fileSubmitted', selectedPhotoFile);

	    // Create a DOM form and add the file to it under the name uploadedphoto
	    var domForm = new FormData();
	    domForm.append('uploadedphoto', selectedPhotoFile);

	    // Using $http to POST the form
	    $http.post('/photos/new', domForm, {
		transformRequest: angular.identity,
		headers: {'Content-Type': undefined}
	    }).then(function successCallback(response){
		// The photo was successfully uploaded. XXX - Do whatever you want on success.
		console.log('broadcasting');
		$rootScope.$broadcast('NewPhotoUploaded');
		$scope.main.updateActivity_photo(response);

		selectedPhotoFile = undefined;
	    }, function errorCallback(response){
		// Couldn't upload the photo. XXX  - Do whatever you want on failure.
		console.error('ERROR uploading photo', response);
	    });

	};


	$scope.main.logout = function () {
		$scope.main.userLoggedIn = false; 
		$scope.current_user_first_name = "";
		$scope.main.current_user_login_name = "";
		$rootScope.$broadcast('LoggedOutBroadcast');
		$location.path("/login-register");
		$scope.main.app_context = "Login";
	};
	
     //old code:
     /*
      * FetchModel - Fetch a model from the web server.
      *   url - string - The URL to issue the GET request.
      *   doneCallback - function - called with argument (model) when the
      *                  the GET request is done. The argument model is the
      *                  objectcontaining the model. model is undefined in
      *                  the error case.
      */
     /*
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
     */

    }]);




  
