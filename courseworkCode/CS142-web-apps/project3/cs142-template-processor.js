'use strict';

var Cs142TemplateProcessor = class Cs142TemplateProcessor {


	constructor(template) {
		this.template = template;
	}


	fillIn(dictionary) {
		
		var curItem;
		var re = /{{[^{}]*}}/g;
		var propertiesInTemplate = this.template.match(re);
	
		
		if (propertiesInTemplate === null || propertiesInTemplate === undefined) {
			return this.template;
		}
			

		//extract the list of properties from the template 
		for (var i = 0; i < propertiesInTemplate.length; i++) {
			curItem = propertiesInTemplate[i];
			var cleanItem = curItem.substring(2, curItem.length - 2);
			propertiesInTemplate[i] = cleanItem;
		}

		//for each property in the template, check to see if it's in the dictionary
		//if it's not, then put in in the dictionary as: ""
		var dictKeys = Object.keys(dictionary);
		for (var j = 0; j < propertiesInTemplate.length; j++) {
			curItem = propertiesInTemplate[i];
			if (dictKeys.indexOf(curItem) < 0) {
				dictionary[curItem] = ""; //template item not defined in dictionary, so define it

			}
		}

		var str = this.template;
		dictKeys = Object.keys(dictionary);

		//for each pair in the dictionary, go through the template string and
		//replace the template with the corresponding value from the dictionary
		for (var k = 0; k < dictKeys.length; k++) {
			var curKey = dictKeys[k];
			var curVal = dictionary[curKey];
			str = str.replace("{{" + curKey + "}}", curVal);	
		}

		return str;
	}

};
