/* 
	
controller.js

Following global variable are required - from private/sparkdevice.js

var url2SparkCore = "https://api.spark.io/v1/devices/<your_device_id>/"
var access_token_param = "access_token=<your_access_token>"

*/

var timeInterval;
var isBagDown = false;
var default_time4tea = "10"; // 10 secs for testing, othewise it should be 180 for 3 minutes


/* -------------------------------------------------------------------------
	Spark core calls & callbacks
*/

function postToDevice(coreMethod, methodParams, callbckFce) {

	var xmlhttp;
	xmlhttp=new XMLHttpRequest();
	xmlhttp.onreadystatechange=function()
  {
	  if (xmlhttp.readyState==4 && xmlhttp.status==200)
	    {
				callbckFce(xmlhttp.response);    		
	    } 
  }
	xmlhttp.open("POST", url2SparkCore + coreMethod ,true);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send(access_token_param + '&params=' + methodParams);
}

function getStatus()
{
	postToDevice("getTimer", "", getStatusCallback);
}

function getStatusCallback(response) {
  var ret_time = JSON.parse(response).return_value;
	if(ret_time<=0) 
		{ 
			clearTimer();
			document.getElementById("timeToTea").innerHTML = 'Ready';
			isBagDown=false;
			updateBagVisual();
		} else {
			time_in_minutes = ~~(ret_time/60)
			document.getElementById("timeToTea").innerHTML = pad(time_in_minutes,2) + ":" + pad(Math.abs(ret_time % 60),2);
			isBagDown=true;
			updateBagVisual();
		}
}


function startYourTea() {
	postToDevice("startTimer", default_time4tea, startYourTeaCallback);
}

function startYourTeaCallback(response) {
		getStatus();
		setTimer();
}

function stopYourTea() {
		postToDevice("stopTimer", "", stopYourTeaCallback);
}

function stopYourTeaCallback(response) {
		getStatus();
		clearTimer(); 
}


/* -------------------------------------------------------------------------
	Timers & events
*/

function myTimer() {	
	getStatus();
}

function setTimer() {
	timeInterval=setInterval(function(){myTimer()},5000);
}

function clearTimer() {
	clearInterval(timeInterval); // don't monitor the status anymore
}


/* -------------------------------------------------------------------------
	onButton callbacks + visual updates
*/

function bagMove() {
	isBagDown = !isBagDown; // time to move the bag

	if(isBagDown) { // move the bag up
		startYourTea();
	} 
	else { // move the bag down
		updateBagVisual();
		stopYourTea();
	}
	
}

function updateBagVisual() {
		document.getElementById("imgBag").style.top = (isBagDown ? "44%" : "0%");
		document.getElementById("btnStart").innerHTML= (isBagDown ? "Stop" : "Start");
}


function pad(num, size) {
	if (!isNaN(num)) {
	    var s = "00" + num;
    	return s.substr(s.length-size);
	} else return "  "
}

				
