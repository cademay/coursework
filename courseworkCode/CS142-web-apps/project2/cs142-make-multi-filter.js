'use strict';

function cs142MakeMultiFilter(originalArray) {



	var arrayFilterer = function arrayFilterer(filterCriteria, callback) {

		if (typeof filterCriteria === "function") {

			var indecesToRemove = [];
			var arrayLength = arrayFilterer.currentArray.length;
			
			//check all elements in array against filterCriteria
			for (var i = 0; i < arrayLength; i++) {
				var currentItem = arrayFilterer.currentArray[i];
				var result = filterCriteria(currentItem);
				
				if (result === false) {
					indecesToRemove.push(i);
				}
			}
	
			//remove elements from the internal array that didn't pass the filterCriteria	
			indecesToRemove.reverse();
	
			for (var j = 0; j < indecesToRemove.length; j++) {
				var index = indecesToRemove[j];
				arrayFilterer.currentArray.splice(index, 1);
			}
			
			//"callback: will be called when the filtering is done"
			if (typeof callback === "function") {
				callback.call(arrayFilterer.originalArray, arrayFilterer.currentArray);
			}


		} else {
			//if filterCriteria is not a function
			return arrayFilterer.currentArray;
		}

		return arrayFilterer;
	};


	// functions are objects too, so they can have properties. we set one here:
	arrayFilterer.currentArray = originalArray;
	arrayFilterer.originalArray = originalArray.slice();

	return arrayFilterer;
}

