'use strict';


cs142App.controller('HeaderController', ['$scope', '$interval', function($scope, $interval) {

 
	$scope.dateEssentials = {
		monthNames: ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"] 
	};

	$scope.dateObject = {
		object: new Date()
	};

	$scope.date = {
		month: $scope.dateEssentials.monthNames[$scope.dateObject.object.getMonth()],
		day: $scope.dateObject.object.getDate(),
		year: $scope.dateObject.object.getFullYear(),
		hours: $scope.dateObject.object.getHours(),
		minutes: $scope.dateObject.object.getMinutes(),
		seconds: $scope.dateObject.object.getSeconds()

	};

	$scope.updateDate = function() {
		$scope.dateObject.object = new Date();
		$scope.date = {
			month: $scope.dateEssentials.monthNames[$scope.dateObject.object.getMonth()],
			day: $scope.dateObject.object.getDate(),
			year: $scope.dateObject.object.getFullYear(),
			hours: $scope.dateObject.object.getHours(),
			minutes: $scope.dateObject.object.getMinutes(),
			seconds: $scope.dateObject.object.getSeconds()
		};

		$scope.time = "The time is probably " + $scope.date.hours + " hours " + $scope.date.minutes + " minutes and " + $scope.date.seconds + " seconds "; 
	};

	$interval($scope.updateDate, 1000);
	
}]);
