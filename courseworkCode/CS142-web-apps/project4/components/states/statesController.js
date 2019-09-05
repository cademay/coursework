
"use strict";
/**
 * Define StatesController for the states component of CS142 project #4
 * problem #2.  The model data for this view (the states) is available
 * at window.cs142models.statesModel().
 */

cs142App.controller('StatesController', ['$scope', function($scope) {

   $scope.query = {
	substring: ""
   };

   $scope.filterStates = function(substr_input) {
	
	var outputList = [];
	var allStates = window.cs142models.statesModel();

	if (substr_input === "") {
		return allStates;
	}

	for (var i = 0; i < allStates.length; i++) {

		var curState = allStates[i];
		var stateIsValid = $scope.isSubstringInString(substr_input, curState);

		if (stateIsValid) {
			outputList.push(curState);	
		}	
	}

	if (outputList.length === 0) {
		outputList.push("No states found");
	}

	return outputList.sort();	
   };

   $scope.validStates = $scope.filterStates($scope.query.substring);

   $scope.isSubstringInString = function(substr, inputStr) {
	
	var lowerStr = inputStr.toLowerCase();
	var lowerSubstr = substr.toLowerCase();

	if (lowerStr.indexOf(lowerSubstr) >= 0) {
		return true;
	} else {
		return false;
	}	
   };

}]);
