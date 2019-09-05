

'use strict';

cs142App.controller('ActivitiesViewController', ['$scope', '$routeParams', '$resource', '$rootScope',
  function($scope, $routeParams, $resource, $rootScope) {
	/*
	* Since the route is specified as '/photos/:userId' in $routeProvider config the
	* $routeParams  should have the userId property set with the path from the URL.
	*/
	var userId = $routeParams.userId;

	var ActivitiesQuery = $resource('/activities/list');
	ActivitiesQuery.query(function (response) {
		
		console.log('response received:');
		console.log(response);

		var clippedList = response.slice(response.length - 5, response.length);
		clippedList.reverse();
	
		console.log(clippedList);	
		$scope.activityList = clippedList;

	}, function (error) {
		console.log(error);
	});	

  }]);
