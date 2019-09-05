cs142App.controller('LoginRegisterController', ['$scope', '$routeParams', '$resource', '$rootScope', '$location',
  function($scope, $routeParams, $resource, $rootScope, $location) {

	$scope.user_login_input = {
		login_name: "",
		password: ""
	};

	$scope.login_button = {};

	$scope.login_issue_display = {
		text: ""

	};

	$scope.login_button.clicked = function () {

		var feed_dict = {
			login_name: $scope.user_login_input.login_name,
			password: $scope.user_login_input.password
		};

		//post request to /admin/login
		$scope.loginPOST(feed_dict);
		
	};


	$scope.loginPOST = function (feed_dict) {

		var LoginRequest = $resource('/admin/login');
		LoginRequest.save(feed_dict, function(loginRequest) {
			// if success:
			//When a user logs in successfully the view should switch to displaying the user's details
			//If a user is logged in, the toolbar should include a small message "Hi <firstname>" where <firstname> is the first name of the logged-in user

			if (loginRequest.hasOwnProperty('_id') && loginRequest._id.length > 0) {

				console.log(loginRequest);
				$scope.main.userLoggedIn = true;
				$scope.main.current_user_first_name = loginRequest.first_name;
				
				var userId = loginRequest._id;
				$scope.main.current_user_userId = userId;
				
				$location.path('/users/' + userId);	
				$rootScope.$broadcast('LoggedInBroadcast');

				$scope.main.userLoggedIn = true;
			} else {
				console.log('else');
				$scope.login_issue_display.text = "Username and password pair does not match our system. Try again.";
			}


		}, function errorHandler(error) {
			// if error:
			// If the user login fails (e.g. no user with the login_name) the view should report an appropriate error message and let the user try again
			console.log(error);
			$scope.login_issue_display.text = "Username and password pair does not match our system. Try again.";
		});
	};


	$scope.registerObject = {
		first_name: "",
		last_name: "",
		login_name: "",
		loc: "",
		description: "",
		occupation: "",
		password1: "",
		password2: ""
	};

	$scope.register_issue_display = {
		text: ""

	};

	$scope.registerObject.register_clicked = function () {
	
		if ($scope.registerObject.password1 !== $scope.registerObject.password2) {
			$scope.register_issue_display.text = "Passwords do not match.";
			return;	
		}

		if ($scope.registerObject.first_name.length <= 0) {
			$scope.register_issue_display.text = "Please enter a first name.";
			return;
		}

		if ($scope.registerObject.last_name.length <= 0) {
			$scope.register_issue_display.text = "Please enter a last name.";
			return;
		}

		if ($scope.registerObject.login_name.length <= 0) {
			$scope.register_issue_display.text = "Please enter a login name.";
			return;
		}


		var RegisterRequest = $resource('/user');

		var feed_dict = {
			login_name: $scope.registerObject.login_name,
			password: $scope.registerObject.password1,
			first_name: $scope.registerObject.first_name,
			last_name: $scope.registerObject.last_name,
			loc: $scope.registerObject.loc,
			description: $scope.registerObject.description,
			occupation: $scope.registerObject.occupation
		};

		RegisterRequest.save(feed_dict, function(response) {

			console.log("re", response);
			$scope.register_issue_display.text = "Registration was a success.";

			var feed_dict = {
				login_name: $scope.registerObject.login_name,
				password: $scope.registerObject.password1
			};

			$scope.loginPOST(feed_dict);

		}, function (error) {
			console.log("err",error);
			$scope.register_issue_display.text = error.data;
		});
	};


}]);
