
'use strict';

class TableTemplate {


	static fillIn(tableID, dict, columnName) {

		var i;
		var j;
		var curRow;
		var processor;   
		var processedStr;
		var template;
		var rowEntries; 

		var column_names = [];
		var tableObject = document.getElementById(tableID); 
		
		var tbody = tableObject.children[0];
		var rows = tbody.children;
		var headerRow = rows[0];
		var headerEntries = headerRow.children;	
		
		for (i = 0; i < headerEntries.length; i++) {
			var curHeaderEntry = headerEntries[i];
			template = curHeaderEntry.textContent;
			processor = new Cs142TemplateProcessor(template);
			processedStr = processor.fillIn(dict);
			curHeaderEntry.textContent = processedStr;
			column_names.push(processedStr);
		}

		if (columnName === undefined || columnName === "" || columnName === null) {
			// if no column name is specified, process entire table

			for (i = 1; i < rows.length; i++) {
				curRow = rows[i];
				rowEntries = curRow.children;
				
				for (j = 0; j < rowEntries.length; j++) {
					template = rowEntries[j].textContent;
					processor = new Cs142TemplateProcessor(template);
					processedStr = processor.fillIn(dict);
					rowEntries[j].textContent = processedStr;
				}
			}	
				
		} else {
			// there is a column name specified

			// if the given columnName is actually a column of the table
			if (column_names.indexOf(columnName) >= 0) {

				var columnIndex = column_names.indexOf(columnName);
				for (i = 1; i < rows.length; i++) {
					curRow = rows[i];
					rowEntries = curRow.children;
					template = rowEntries[columnIndex].textContent;
					processor = new Cs142TemplateProcessor(template);
					processedStr = processor.fillIn(dict);
					rowEntries[columnIndex].textContent = processedStr;
				}	
			}
		}

		//do this last
		if (tableObject.style.visibility === "hidden") {
			tableObject.style.visibility = "visible";
		}
	}

}
