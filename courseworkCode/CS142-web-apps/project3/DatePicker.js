'use strict';


class DatePicker {

	constructor(id, dateSelectionCallbackFunction) {
		this.id = id;
		this.callback = dateSelectionCallbackFunction;
	}


	//when date is selected:
	//dateSelectionCallbackFunction(id, date);


}

DatePicker.prototype.render = function render(dateObject) {

	var finalHTML = "";
	var li;
	var curDay;
	var picker = document.getElementById(this.id);

	var monthNames = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"];

	var currentYear = dateObject.getFullYear();
	var month_year = monthNames[dateObject.getMonth()] + " " + currentYear;

	finalHTML += '<div class="outer">';
	finalHTML += '<div class="buttonDiv">';
	finalHTML += '<button type="button" id="arrowPrev' + this.id + '">' + '<' + '</button>';
	finalHTML += month_year;
	finalHTML += '<button type="button" id="arrowNext' + this.id + '">' + '>' + '</button>';
	finalHTML += '</div>';

	finalHTML += '<div class="lists">';	
	//build weekday header: looks like: SU MO TU WE... etc
	var weekDays = ["Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"];
	var weekdayHTML = '<ul class="weekdays">';
	for (var i = 0; i < weekDays.length; i++) {	
		li = '<li class="weekday">' + weekDays[i] + '</li>';
		weekdayHTML += li;
	}
	weekdayHTML += '</ul>';
	finalHTML += weekdayHTML;
	


	var beginningOfMonth = new Date(currentYear, dateObject.getMonth() , 1);
	var endOfMonth = new Date(currentYear, dateObject.getMonth() + 1, 1);
	var monthLength = Math.round((endOfMonth - beginningOfMonth) / (1000 * 60 * 60 * 24));

	var weekdayOfFirstDayOfMonth = beginningOfMonth.getDay();


	var prevMonthIndex;
	var prevMonthYear;
	if (dateObject.getMonth() === 0) {
		//if january, wrap around to december of prev year
		prevMonthIndex = 11;
		prevMonthYear = currentYear - 1;
	} else {
		//otherwise, don't wrap around
		prevMonthIndex = dateObject.getMonth() - 1;
		prevMonthYear = currentYear;
	}		


	
	
	var beginningOfPrevMonth = new Date(prevMonthYear, prevMonthIndex , 1);
	var endOfPrevMonth = new Date(prevMonthYear, prevMonthIndex + 1, 1);
	var prevMonthLength = Math.round((endOfPrevMonth - beginningOfPrevMonth) / (1000 * 60 * 60 * 24));


	var week1HTML = '<ul class="week">';

	for (i = prevMonthLength - weekdayOfFirstDayOfMonth + 1; i <= prevMonthLength; i++) {
		li = '<li class="off_day">' + i + '</li>';
		week1HTML += li;
	}

	var daysToAdd = [];
	for (i = 1; i <= monthLength; i++) {
		daysToAdd.push(i);
	}
	
	var thisMonthEnd = new Date(endOfMonth.getTime() - 1000*60*60*24);
	var tailDays = 6 - (thisMonthEnd.getDay());

	for (i = 1; i <= tailDays; i++) {
		daysToAdd.push(i);
	}
	
	daysToAdd = daysToAdd.reverse();

	for (i = weekdayOfFirstDayOfMonth; i < 7; i++) {
		curDay = daysToAdd.pop();
		li = '<li class="day">' + curDay + '</li>';
		week1HTML += li;
	}

	week1HTML += '</ul>';
	finalHTML += week1HTML;

	var offCounter = 0;
	while (daysToAdd.length > 0) {

		var week = '<ul class="week">';
		for (i = 0; i < 7; i++) {

			curDay = daysToAdd.pop();
			if (offCounter > 15 && curDay < 8) {
				//if the day belongs to the next month, it's an off day
				li = '<li class="off_day">' + curDay + '</li>';
			} else {
				//if it belongs to this month, it's just a day
				li = '<li class="day">' + curDay + '</li>';
			}
			week += li;
			offCounter += 1;
		}
		week += '</ul>';
		finalHTML += week;
	}

	finalHTML += '</div>';
	finalHTML += '</div>';
	picker.innerHTML = finalHTML; 	



	var arrowPrev = document.getElementById("arrowPrev" + this.id);	
	var arrowNext = document.getElementById("arrowNext" + this.id);	

	var thishack = this;	
	arrowPrev.addEventListener("click", function() {
		
		var currentYear = dateObject.getFullYear();
		var prevMonthIndex;
		var prevMonthYear;

		if (dateObject.getMonth() === 0) {
			//if january, wrap around to december of prev year
			prevMonthIndex = 11;
			prevMonthYear = currentYear - 1;
		} else {
			//otherwise, don't wrap around
			prevMonthIndex = dateObject.getMonth() - 1;
			prevMonthYear = currentYear;
		}		

		var newDate = new Date(prevMonthYear, prevMonthIndex, 1);
		render.call(thishack, newDate);
	});

	arrowNext.addEventListener("click", function() {
		
		var currentYear = dateObject.getFullYear();
		var nextMonthIndex;
		var nextMonthYear;

		if (dateObject.getMonth() === 11) {
			//if december, wrap around to january of next year
			nextMonthIndex = 0;
			nextMonthYear = currentYear + 1;
		} else {
			//otherwise, don't wrap around
			nextMonthIndex = dateObject.getMonth() + 1;
			nextMonthYear = currentYear;
		}		
		var newDate = new Date(nextMonthYear, nextMonthIndex, 1);

		render.call(thishack, newDate);
	});

	var callbackFunction = this.callback;
	var ID = this.id;
	var children = picker.getElementsByClassName("day");
	for (i = 0; i < children.length; i++) {
		children[i].addEventListener("click", function () {
	
			var date_selected = {};
			
			date_selected.month = dateObject.getMonth() + 1;
			date_selected.day = this.innerHTML;
			date_selected.year = dateObject.getFullYear();
			
			callbackFunction(ID, date_selected);
		});
	}

	/*
	var numberDays = document.getElementsByClass("day");

	for (i = 0; i < numberDays.length; i++) {
		
	
	}
	*/
};



