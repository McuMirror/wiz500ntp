#ifndef _WEBPAGE_H_
#define _WEBPAGE_H_

/*************************************************************************************
 * JavaScript Functions
 *************************************************************************************/

#define wiz550web_ajax_js 	"function AJAX(a,e){var c=d();c.onreadystatechange=b;function d(){if(window.XMLHttpRequest){return new XMLHttpRequest()}else{if(window.ActiveXObject){return new ActiveXObject(\"Microsoft.XMLHTTP\")}}}function b(){if(c.readyState==4){if(c.status==200){if(e){e(c.responseText)}}}}this.doGet=function(){c.open(\"GET\",a,true);c.send(null)};this.doPost=function(f){c.open(\"POST\",a,true);c.setRequestHeader(\"Content-Type\",\"application/x-www-form-urlencoded\");c.setRequestHeader(\"ISAJAX\",\"yes\");c.send(f)}}function $(a){return document.getElementById(a)}function $$(a){return document.getElementsByName(a)}function $$_ie(a,c){if(!a){a=\"*\"}var b=document.getElementsByTagName(a);var e=[];for(var d=0;d<b.length;d++){att=b[d].getAttribute(\"name\");if(att==c){e.push(b[d])}}return e}"

/* Get: Network Information: function NetinfoCallback(o), getNetinfo() */
#define wiz550web_netinfo_js	"function NetinfoCallback(o){"\
									 "$('txtmac').value=o.mac;"\
									 "$('txtip').value=o.ip;"\
									 "$('txtgw').value=o.gw;"\
									 "$('txtsn').value=o.sn;"\
									 "$('txtdns').value=o.dns;"\
									 "if(typeof(window.external)!='undefined'){"\
										"obj=$$_ie('input','dhcp');"\
									 "}else{"\
										"obj=$$('dhcp');"\
									 "}"\
								"}"\
								" "\
								"function getNetinfo(){"\
									 "var oUpdate;"\
									 "setTimeout(function(){"\
									 	"oUpdate=new AJAX('get_netinfo.cgi',function(t){"\
									 		"try{eval(t);}catch(e){alert(e);}"\
									 "});"\
									 "oUpdate.doGet();},300);"\
								 "}"


/* Get: Digital I/O state/dir:  function DioCallback(o), getDio(o) */
/* Set: Digital I/O state: 		function setDiostate(o) */
/* Set: Digital I/O direction: 	function setDiodir(o) */
#define wiz550web_dio_js	"function DioCallback(o){"\
								"var pin = o.dio_p;"\
								"$('txtdio_s'+pin).value=o.dio_s;"\
								"$('txtdio_d'+pin).value=o.dio_d;"\
							"}"\
							"function getDio(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"oUpdate=new AJAX('get_dio'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();"\
							"}"\
							" "\
							"function setDiostate(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_s'+p).value;*/"\
								"var v=o.attributes['s'].value;"\
								"dout=new AJAX('set_diostate.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"\
							" "\
							"function setDiodir(o){"\
								"var p=o.attributes['pin'].value;"\
								"/*var v=$('txtdio_d'+p).value;*/"\
								"var v=o.attributes['d'].value;"\
								"dout=new AJAX('set_diodir.cgi', function(t){try{eval(t);}catch(e){alert(e);}});"\
								"dout.doPost('pin='+p+'&val='+v);"\
							"}"


/* Get: Analog Input: 	function AinCallback(o), getAin(o) */
/* Additional function: function AinDrawgraph(o), getAin6_update() */
#define wiz550web_ain_js	"function AinCallback(o){"\
								"var pin = o.ain_p;"\
								"$('txtain_v'+pin).value=o.ain_v;"\
								"AinDrawgraph(o);"\
							"}"\
							"function getAin(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
							"}"\
							"function AinDrawgraph(o){"\
								"var pin=o.ain_p;"\
								"var val=o.ain_v;"\
								"$('ain_v'+pin).style.width=val*500/1023+'px';"\
							"}"\
							"function getAin6_update() {"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain6.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
								"setTimeout('getAin6_update()',500);"\
							"}"


/* Get: Analog Input: 	function AinCallback(o), getAin(o) */
/* Additional function: function AinDrawGoogleGauge(o), getAin6_update() */
#define ain_gauge_js		"google.load('visualization', '1', {packages:['gauge']});"\
							"google.setOnLoadCallback(AinDrawGoogleGauge);"\
							"function AinCallback(o){"\
								"var pin = o.ain_p;"\
								"$('txtain_v'+pin).value=o.ain_v;"\
								"AinDrawGoogleGauge(o);"\
							"}"\
							"function getAin(o) {"\
								"var p=o.attributes['pin'].value;"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain'+p+'.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
							"}"\
							"function AinDrawGoogleGauge(o){"\
								"var val=o.ain_v;"\
								"var temp_val=Number(((((val*3300)/1023)-500)/10).toFixed(2));"\
								"var data = google.visualization.arrayToDataTable(["\
									"['Label', 'Value'],"\
									"['Temp.', 80]"\
								"]);"\
								"var options = {"\
									"width:400, height:120,"\
									"max:100, min:-40,"\
									"greenFrom:-40, greenTo:0,"\
									"redFrom:90, redTo:100,"\
									"yellowFrom:75, yellowTo:90,"\
									"minorTicks: 5"\
								"};"\
								"var chart = new google.visualization.Gauge(document.getElementById('chart_div'));"\
								"data.setValue(0, 1, temp_val);"\
								"chart.draw(data, options);"\
							"}"\
							"function getAin6_update() {"\
								"var oUpdate;"\
								"setTimeout(function(){"\
									"oUpdate=new AJAX('get_ain6.cgi',function(t){try{eval(t);}catch(e){alert(e);}});"\
								"oUpdate.doGet();},300);"\
								"setTimeout('getAin6_update()',500);"\
							"}"
//favicon.ico							
#define pageico "\0" 

/*************************************************************************************
 * HTML Pages (web pages)
 *************************************************************************************/
#define index_page1			"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"</head>"\
								"<body>"\
									"<div>"\
									"W5500-EVB Web Server Demopage"\
									"</div>"\
									"<br>"\
									"<a href='netinfo.html'>Network Information</a>"\
									"<br>"\
									"<a href='img.html'>Base64 Image Data</a>"\
									"<br>"\
									"<br>"\
									"<a href='dio.html'>Ex1> Digital I/O</a>"\
									"<br>"\
									"<a href='ain.html'>Ex2> Analog Input</a>"\
									"<br>"\
									"<a href='ain_gauge.html'>Ex3> Analog Input: Google Gauge Chart</a>"\
									"<script>"\
									"setInterval(calldata, 5000);"\
									"function calldata(){"\
									"var xhttp = new XMLHttpRequest();"\
									"xhttp.onreadystatechange = function() {"\
									"if (this.readyState == 4 && this.status == 200) {"\
									"var txt = this.responseText;"\
									"var obj = JSON.parse(txt);"\
									"document.getElementById('txtmac').value = Math.round(obj.volt);"\
									"document.getElementById('txtip').value = obj.Rain ;"\
									"document.getElementById('txtgw').value = Math.round(obj.Temperature) ;"\
									"document.getElementById('txtdns').value = Math.round(obj.Temperature) ;"\
									"}"\
									"};"\
									"xhttp.open('GET', 'alldata.json', true); "\
									"xhttp.send();"\
									"}"\
									"</script>"\
								"</body>"\
							"</html>"
							
#define index_page2			"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"</head>"\
								"<body>"\
									"<div>"\
									"W5500-EVB Web Server Demopage"\
									"</div>"\
									"<br>"\
									"<a href='netinfo.html'>Network Information</a>"\
									"<br>"\
									"<a href='img.html'>Base64 Image Data</a>"\
									"<br>"\
									"<br>"\
									"<a href='dio.html'>Ex1> Digital I/O</a>"\
									"<br>"\
									"<a href='ain.html'>Ex2> Analog Input</a>"\
									"<br>"\
									"<a href='ain_gauge.html'>Ex3> Analog Input: Google Gauge Chart</a>"\
								"</body>"\
							"</html>"							
#define index_page	"<!DOCTYPE html>"\
"<html>"\
	"<head>"\
	"<title>GPS Master clock configuration</title>"\
	"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
		"<style>"\
			"body{font-family: Arial, Tahoma;font-size: 14px;}"\
 			"h1{color:black;font-size:18px;text-align:center;}"\
			".btn {display: block;text-align: center;cursor: pointer;text-transform: uppercase;overflow: hidden;color: #fff;background-color: #D2691E;height: 30px;width: 80px;margin: auto;border-radius: 10pt}"\
			".btn:hover {background-color: #E9967A;}"\
			".tab{font-family:Arial; width: 498px; align : center; border: 0px; cellpadding:5px; cellspacing: 0px; height:50px;}"\
			".sts_label{border: none;color: black;font-size: 15px;height: 30px;width: 60px;background: #F5F5DC;}"\
			"#clock_info{height: 140px;border: 1px solid #CDCDCD;background-color: #F5F5DC;margin-bottom:5px;clear: both;text-align:center;}"\
			"#clockdiv{color: #fff;display: inline-block;font-weight: 100px;text-align: center;font-size: 30px;}"\
			"#clockdiv > div{padding: 10px;border-radius: 3px;background: #00BF96;display: inline-block;text-align: center; height:70px;width: 55px;}"\
			".clock_label{padding: 5px;border-radius: 3px;background: #00816A;display: inline-block; height: 20px; width: 40px; color:white;overflow: hidden;font-weight: bold; font-size: 16px;text-align: center;}"\
			".smalltext{padding-top: 5px;font-size: 16px;}"\
			"#main{width: 500px;border: 1px solid #6495ED;padding: 0;margin-left: auto;margin-right: auto;}"\
			"#head{border: 1px solid #CDCDCD;height: 70px;background-color:#008800;color:white;overflow: hidden;font-weight: bold;font-size:30px;text-align:center;margin-bottom:5px;}"\
			"#net_info_general{min-height: 150px;border: 1px solid #CDCDCD;float:left;margin-bottom:5px;background-color: #F5F5DC;}"\
			"#list_head{background-color:#FFCC99;height: 40px;width: 100%; display: inline-block;font-size: 18px;}"\
			"#info{height: 90px;border: 1px solid #CDCDCD;background-color: #F5F5DC;margin-bottom:5px;clear: both;}"\
			"#foot{background-color: #0099FF;border:1px solid #ccc;color:black;font-size:15px;height: 20px;overflow: hidden;padding-top: 5px;padding-bottom: 5px;text-align:center;}"\
		"</style>"\
	"</head>"\
	"<body>"\
		"<div id='main'>"\
			"<div id='head'>"\
				"GPS MASTER CLOCK CONFIGURATION"\
			"</div>"\
			"<div id='net_info_general'>"\
				"<div id = 'list_head'>"\
					"<h1>Network information</h1>"\
				"</div>"\
				"<table type ='table' class='tab'>"\
					"<tbody>"\
						"<tr>"\
							"<td>IP adress:&nbsp</td>"\
							"<td colspan='2'><input id='txtip' name='ip' type='text' disabled='disabled'/></td>"\
						"</tr>"\
						"<tr>"\
							"<td>Subnet mask:&nbsp</td>"\
							"<td colspan='2'><input id='txtsn' name='sn' type='text' disabled='disabled'/></td>"\
						"</tr>"\
					"</tbody>"\
				"</table>"\
				"<button type='button' class ='btn' onclick=location.href='config.html'>Setup</button>"\
			"</div>"\
			"<div id = 'clock_info'>"\
				"<div id = 'list_head'>"\
					"<h1>GPS time</h1>"\
				"</div>"\
				"<table type ='table' class='tab'>"\
					"<tbody>"\
						"<tr>"\
							"<td>"\
								"<div id='clockdiv'>"\
									"<div>"\
										"<input type='text' class='clock_label' id='txtdays' name='days' disabled='disabled' value=' '/>"\
										"<div class='smalltext'>Days</div>"\
									"</div>"\
									"<div>"\
										"<input type='text' class='clock_label' id='txtmonths' name='months' disabled='disabled' value=' '/>"\
										"<div class='smalltext'>Months</div>"\
									"</div>"\
									"<div>"\
										"<input type='text' class='clock_label' id='txtyears' name='years' disabled='disabled' value=' '/>"\
										"<div class='smalltext'>Years</div>"\
									"</div>"\
									"<div>"\
										"<input type='text' class='clock_label' id='txthours' name='hours' disabled='disabled' value=' '/>"\
										"<div class='smalltext'>Hours</div>"\
									"</div>"\
									"<div>"\
										"<input type='text' class='clock_label' id='txtminutes' name='minutes' disabled='disabled' value=' '/>"\
										"<div class='smalltext'>Minutes</div>"\
									"</div>"\
									"<div>"\
										"<input type='text' class='clock_label' id='txtseconds' name='seconds' disabled='disabled' value=' '/>"\
										"<div class='smalltext'>Seconds</div>"\
									"</div>"\
								"</div>"\
							"</td>"\
						"</tr>"\
					"</tbody>"\
				"</table>"\
			"</div>"\
			"<div id = 'info'>"\
				"<div id = 'list_head'>"\
				"<h1>GPS status</h1>"\
				"</div>"\
				"<table type ='table' class='tab'>"\
					"<tbody>"\
						"<tr>"\
							"<td style='color: #000000; width:25%; align:center'>GPS 01:&nbsp</td>"\
							"<td style='color: #000000; width:15%; align: left'><input type='text' class='sts_label' id='txtgps01' name='gps01' disabled='disabled'/></td>"\
							"<td style='color: #000000; width:20%'>"\
							"<td style='color: #000000; width:25%; align:center'>GPS 02:&nbsp</td>"\
							"<td style='color: #000000; width:15%; align:left'><input type='text' class='sts_label' id='txtgps02' name='gps02' disabled='disabled'/></td>"\
						"</tr>"\
					"</tbody>"\
				"</table>"\
			"</div>"\
			"<div id = 'info'>"\
				"<div id = 'list_head'>"\
				"<h1>Power supply status</h1>"\
				"</div>"\
				"<table type ='table' class='tab'>"\
					"<tbody>"\
						"<tr>"\
							"<td style='color: #000000; width:25%; align:center'>Power supply 01:&nbsp</td>"\
							"<td style='color: #000000; width:15%; align: left'><input type='text' class='sts_label' id='txtpower01' name='power01' disabled='disabled'/></td>"\
							"<td style='color: #000000; width:20%'>"\
							"<td style='color: #000000; width:25%; align:center'>Power supply 02:&nbsp</td>"\
							"<td style='color: #000000; width:15%; align:left'><input type='text' class='sts_label' id='txtpower02' name='power02' disabled='disabled'/></td>"\
						"</tr>"\
					"</tbody>"\
				"</table>"\
			"</div>"\
			"<div id = 'info'>"\
				"<div id = 'list_head'>"\
				"<h1>Device version</h1>"\
				"</div>"\
				"<table type ='table' class='tab'>"\
					"<tbody>"\
						"<tr>"\
							"<td style='color: #000000; width:30%; align:center'>Hardware version:&nbsp</td>"\
							"<td style='color: #000000; width:10%; align: left'><input type='text' class='sts_label' value='Ver 2.1.1' disabled='disabled'/></td>"\
							"<td style='color: #000000; width:20%'>"\
							"<td style='color: #000000; width:30%; align:center'>Software version:&nbsp</td>"\
							"<td style='color: #000000; width:10%; align:left'><input type='text' class='sts_label' value='Ver 2.1.2' disabled='disabled'/></td>"\
						"</tr>"\
					"</tbody>"\
				"</table>"\
			"</div>"\
			"<div id = 'foot'>"\
				"Powered by <a style='color: #000000; font-weight: bold; text-decoration: none'>Air Traffic Technical Company - <span style='color:red'>A</span><span style='color: blue;'>TTECH</span></a>"\
			"</div>"\
		"</div>"\
		"<script>"\
			"setInterval(calldata, 1000);"\
			"function calldata(){"\
				"var xhttp = new XMLHttpRequest();"\
				"xhttp.onreadystatechange = function() {"\
					"if (this.readyState == 4 && this.status == 200) {"\
						"var txt = this.responseText;"\
						"var obj = JSON.parse(txt);"\
						"document.getElementById('txtip').value = obj.txtip;"\
						"document.getElementById('txtsn').value = obj.txtsn;"\
						"document.getElementById('txtdays').value = obj.txtdays;"\
						"document.getElementById('txtmonths').value = obj.txtmonths;"\
						"document.getElementById('txtyears').value = obj.txtyears;"\
						"document.getElementById('txthours').value = obj.txthours;"\
						"document.getElementById('txtminutes').value =obj.txtminutes;"\
						"document.getElementById('txtseconds').value = obj.txtseconds;"\
						"document.getElementById('txtgps01').value = obj.txtgps01;"\
						"document.getElementById('txtgps02').value = obj.txtgps02;"\
						"document.getElementById('txtpower01').value = obj.txtpower01;"\
						"document.getElementById('txtpower02').value = obj.txtpower02;"\
					"}"\
				"};"\
				"xhttp.open('GET', 'alldata.json', true); "\
				"xhttp.send();"\
			"}"\
		"</script>"\
	"</body>"\
"</html>"

#define netinfo_page 		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Network Info</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<style>"\
								"label{float:left;text-align:left;width:50px;}"\
								"li {list-style:none;}"\
								"</style>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='netinfo.js'></script>"\
								"</head>"\
								"<body onload='getNetinfo();'>"\
									"<div>"\
									"W5500-EVB Web Server Network Information"\
									"</div>"\
									"<br>"\
									"<ul>"\
										"<li><label for='txtmac'>MAC:</label><input id='txtmac' name='mac' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtip'>IP:</label><input id='txtip' name='ip' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtgw'>GW:</label><input id='txtgw' name='gw' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtsn'>SN:</label><input id='txtsn' name='sn' type='text' size='20' disabled='disabled'/></li> "\
										"<li><label for='txtdns'>DNS:</label><input id='txtdns' name='dns' type='text' size='20' disabled='disabled'/></li> "\
									"</ul>"\
								"</body>"\
							"</html>"


#define dio_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Digital I/O</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='dio.js'></script>"\
								"</head>"\
								"<body>"\
									"<!-- to do -->"\
									"<div>"\
									"<input type='button' value='LED R On' pin='8' s='0' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED R Off' pin='8' s= '1' onclick='setDiostate(this);'>"\
									"<br>"\
									"<input type='button' value='LED G On' pin='9' s='0' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED G Off' pin='9' s= '1' onclick='setDiostate(this);'>"\
									"<br>"\
									"<input type='button' value='LED B On' pin='10' s='0' onclick='setDiostate(this);'> "\
									"<input type='button' value='LED B Off' pin='10' s= '1' onclick='setDiostate(this);'>"\
									"</div>"\
									"<!-- to do -->"\
								"</body>"\
							"</html>"


#define ain_page  			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Analog Input</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
 	 	 	 	 	 	 	 	"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='ain.js'></script>"\
								"<style>"\
									".analog{margin-top:2px;margin-right:10px;border:1px solid #ccc;height:20px;width:500px;display:block;}"\
									".ain{width:0%;height:100%;text-align:center;background:red;float:left;display:block;}"\
								"</style>"\
								"</head>"\
								"<body>"\
									"<!-- to do -->"\
									"<div>"\
										"<input type='text' id='txtain_v6' size='5' disabled='disabled' value=''> "\
										"<input type='button' value='Get AIN' pin='6' onclick='getAin(this);'> "\
										"<input type='button' value='Get AIN Auto' onclick='getAin6_update();'> "\
										"<br>"\
										"<div class='analog' style='padding:0px;'><strong id='ain_v6' name='ain' class='ain' style='width:0%;'></strong></div>"\
									"</div>"\
									"<!-- to do -->"\
								"</body>"\
							"</html>"



#define ain_gauge_page		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Analog Input Gauge</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
 	 	 	 	 	 	 	 	"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='https://www.google.com/jsapi'></script>"\
								"<script type='text/javascript' src='ain_gauge.js'></script>"\
								"</head>"\
								"<body>"\
									"<div>"\
										"<input type='text' id='txtain_v6' size='5' disabled='disabled' value=''> "\
										"<input type='button' value='Get AIN' pin='6' onclick='getAin(this);'> "\
										"<input type='button' value='Get AIN Auto' onclick='getAin6_update();'> "\
										"<br>"\
										"<!--Draw the Google Gauge Chart-->"\
										"<div id='chart_div' style='width: 400px; height: 120px;'></div>"\
									"</div>"\
								"</body>"\
							"</html>"


/* Base64 Image converter: http://base64.wutils.com/encoding-online/ */
#define img_page			"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>W5500-EVB Web Server Image</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8' />"\
								"<meta http-equiv='pragma' content='no-cache' />"\
								"<meta http-equiv='content-type' content='no-cache, no-store, must-revalidate' />"\
								"</head>"\
								"<body>"\
									"<div>"\
									"<img alt='wizwebserver.png'"\
									"src='data:image/png;base64,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' />"\
									"</div>"\
								"</body>"\
							"</html>"
							
#define configpage			"<!DOCTYPE html>"\
"<html>"\
"<body><CENTER>"\
"<h3>Cau hinh IP </h3>"\
"<FORM METHOD='POST' action='config.cgi'><br>"\
"<input type=\"text\" name=\"ip\" value=\"192.168.1.246\"> DeviveIP<br>"\
"<input type=\"text\" name=\"gw\" value=\"192.168.1.1\"> Getway<br>"\
"<input type=\"text\" name=\"sn\" value=\"255.255.255.0\"> Subnet <br>"\
"<input type=\"submit\" value=\"Gui\">"\
"</form>"\
"</CENTER></body>"\
"</html>"\

#endif
