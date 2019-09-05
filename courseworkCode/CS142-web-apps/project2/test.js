var template = 'My favorite month is {{month}} but not the day {{day}} or the year {{year}}';
var dateTemplate = new Cs142TemplateProcessor(template);

var dictionary = {month: 'July', day: '1', year: '2016'};
var str = dateTemplate.fillIn(dictionary);


console.log(str);
